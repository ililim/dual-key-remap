#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include "input.h"
#include "keys.c"
#include "remap.c"

// Globals
// ----------------

// A semi random value used to identify inputs generated
// by Dual Key Remap. Ideally high to minimize chances of a collision
// with a real pointer used by another application.
// Note: This approach is what AHK used, we should a different key id
// from them to avoid collisions.
#define INJECTED_KEY_ID 0xFFC3CED7

struct Remap * g_remap_list;
HHOOK g_keyboard_hook;
HHOOK g_mouse_hook;

void send_input(int scan_code, int virt_code, enum Direction direction)
{
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.time = 0;
    input.ki.dwExtraInfo = (ULONG_PTR)INJECTED_KEY_ID;

    input.ki.wScan = scan_code;
    input.ki.wVk = virt_code;
    input.ki.dwFlags = direction == UP ? KEYEVENTF_KEYUP : 0;

    SendInput(1, &input, sizeof(INPUT));
}


LRESULT CALLBACK mouse_callback(int msg_code, WPARAM w_param, LPARAM l_param) {
    int swallow_input = 0;

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
            swallow_input = handle_input(0, 0, 0, 0);
        }
    }

    return (swallow_input) ? 1 : CallNextHookEx(g_mouse_hook, msg_code, w_param, l_param);
}

LRESULT CALLBACK keyboard_callback(int msg_code, WPARAM w_param, LPARAM l_param)
{
    int swallow_input = 0;

    // Per MS docs we should only act for HC_ACTION's
    if (msg_code == HC_ACTION) {
        KBDLLHOOKSTRUCT * data = (KBDLLHOOKSTRUCT *)l_param;
        enum Direction direction = (w_param == WM_KEYDOWN || w_param == WM_SYSKEYDOWN)
            ? DOWN
            : UP;
        int is_injected = data->dwExtraInfo == INJECTED_KEY_ID;
        swallow_input = handle_input(
            data->scanCode,
            data->vkCode,
            direction,
            is_injected
        );
    }

    return (swallow_input) ? 1 : CallNextHookEx(g_mouse_hook, msg_code, w_param, l_param);
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
    HMODULE module = GetModuleHandleW(NULL);
    GetModuleFileNameW(module, path, MAX_PATH);
    path[wcslen(path) - strlen("main.exe")] = '\0';
    wcscat(path, L"config.txt");
}


int main()
{
    printf("dual-key-remap.exe version: 0.4, author: ililim\n\n");

    HWND window = GetConsoleWindow();
    HANDLE mutex = CreateMutex(NULL, TRUE, "dual-key-remap.single-instance");
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

    g_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_callback, NULL, 0);
    g_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_callback, NULL, 0);

    // No errors, hide the console window if we're not debugging
	if (!g_debug)
	{
		ShowWindow(window, SW_HIDE);
	}

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    end:
        // ShowWindow(window, SW_SHOW);
        printf("\nPress any key to exit...\n");
        getch();
        return 1;
}
