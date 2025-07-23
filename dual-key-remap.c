#define VERSION "0.9"
#define AUTHOR "ililim"

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include "input.h"
#include "keys.c"
#include "remap.c"
#include "tray.c"

// Globals
// ----------------

// A semi random value used to identify inputs generated
// by Dual Key Remap. Ideally high to minimize chances of a collision
// with a real pointer used by another application.
// Note: This approach is what AHK used, we should a different key id
// from them to avoid collisions.
#define INJECTED_KEY_ID 0xFFC3CED7

struct Remap * g_remap_list;
HHOOK g_mouse_hook;
HHOOK g_keyboard_hook;

void send_input(int scan_code, int virt_code, enum Direction direction)
{
    if (scan_code == 0 && virt_code == 0) return;

    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.time = 0;
    input.ki.dwExtraInfo = (ULONG_PTR)INJECTED_KEY_ID;

    input.ki.wScan = scan_code;
    input.ki.wVk = virt_code;
    // Per MS Docs: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-keybd_even
    // we need to flag whether "the scan code was preceded by a prefix byte having the value 0xE0 (224)"
    int is_extended_key = scan_code>>8 == 0xE0;
    input.ki.dwFlags = (direction == UP ? KEYEVENTF_KEYUP : 0) |
        (is_extended_key ? KEYEVENTF_EXTENDEDKEY : 0);

    SendInput(1, &input, sizeof(INPUT));
}

LRESULT CALLBACK mouse_callback(int msg_code, WPARAM w_param, LPARAM l_param) {
    int block_input = 0;

    // Per MS docs we should only act for HC_ACTION's
    if (msg_code == HC_ACTION) {
        switch (w_param) {
        case WM_MOUSEWHEEL:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_NCXBUTTONDOWN:
        case WM_XBUTTONDOWN:
            // Since no key corresponds to the mouse inputs; use a dummy input
            block_input = handle_input(0, MOUSE_DUMMY_VK, 0, 0);
        }
    }

    return (block_input) ? 1 : CallNextHookEx(g_mouse_hook, msg_code, w_param, l_param);
}

LRESULT CALLBACK keyboard_callback(int msg_code, WPARAM w_param, LPARAM l_param)
{
    int block_input = 0;

    // Per MS docs we should only act for HC_ACTION's
    if (msg_code == HC_ACTION) {
        KBDLLHOOKSTRUCT * data = (KBDLLHOOKSTRUCT *)l_param;
        enum Direction direction = (w_param == WM_KEYDOWN || w_param == WM_SYSKEYDOWN)
            ? DOWN
            : UP;
        int is_injected = data->dwExtraInfo == INJECTED_KEY_ID;
        block_input = handle_input(
            data->scanCode,
            data->vkCode,
            direction,
            is_injected
        );
    }

    return (block_input) ? 1 : CallNextHookEx(g_mouse_hook, msg_code, w_param, l_param);
}

static void ensure_capslock_off(void) {
    SHORT state = GetKeyState(VK_CAPITAL);
    if (state & 1) {
        printf("Detected capslock active: toggling it off...\n");
        USHORT scan = (USHORT)MapVirtualKey(VK_CAPITAL, MAPVK_VK_TO_VSC);
        // send injected CapsLock DOWN + UP to clear toggle
        send_input(scan, VK_CAPITAL, DOWN);
        send_input(scan, VK_CAPITAL, UP);
    }
}


void create_console()
{
    if (AllocConsole()) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        printf("== dual-key-remap (version: %s, author: %s) ==\n\n", VERSION, AUTHOR);
    }
}

void destroy_console()
{
    fclose(stdout);
    fclose(stderr);
    FreeConsole();
}

int load_config_file(wchar_t * path)
{
    FILE * file;
    char line[255];

    if (_wfopen_s(&file, path, L"r") > 0) {
        printf("Cannot open configuration file '%ws'. Make sure it is in the same directory as 'dual-key-remap.exe'.\n",
            path);
        return 1;
    }

    int linenum = 1;
    while (fgets(line, 255, file)) {
        if (load_config_line((char *)&line, linenum++)) {
            fclose(file);
            return 1;
        }
    };
    fclose(file);
    return 0;
}

void put_config_path(wchar_t * path)
{
    HMODULE module = GetModuleHandleW(0);
    GetModuleFileNameW(module, path, MAX_PATH);
    path[wcslen(path) - strlen("dual-key-remap.exe")] = '\0';
    wcscat(path, L"config.txt");
}


int main()
{
    // Initialization may print errors to stdout, create a console to show that output
    create_console();

    HANDLE mutex = CreateMutex(0, TRUE, "dual-key-remap.single-instance");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        printf("dual-key-remap.exe is already running!\n");
        goto end;
    }

    wchar_t config_path[MAX_PATH];
    put_config_path(config_path);
    int err = load_config_file(config_path);
    if (err) {
        goto end;
    }

    // Elevate process and main thread priority to reduce input lag under high CPU load
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    g_debug = g_debug || getenv("DEBUG");
    g_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_callback, 0, 0);
    g_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_callback, 0, 0);

    if (!g_mouse_hook || !g_keyboard_hook) {
        printf("Failed to set keyboard or mouse hooks, aborting.\n");
        goto end;
    }

    // We're all good if we got this far, hide the console window unless we're debugging
    if (g_debug) {
        printf("-- DEBUG MODE --\n");
    } else {
        destroy_console();
    }


    // If we're remapping capslock, clear its state so we won't start stuck on
    if (find_remap_for_virt_code(VK_CAPITAL)) {
        ensure_capslock_off();
    }

    // Initialize tray icon
    if (!init_tray_icon()) {
        if (g_debug) printf("Failed to create tray icon\n");
    }

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    cleanup_tray_icon();

end:
    printf("Press any key to exit...\n");
    getchar();
    return 0;
}
