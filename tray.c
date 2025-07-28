#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
// #include "resource.h"  // Uncomment when using embedded resources

// Tray icon constants
#define WM_TRAY_ICON_MESSAGE (WM_USER + 1)
#define TRAY_ICON_ID 1001

// Menu command IDs
#define MENU_PAUSE_RESUME    2001
#define MENU_DEBUG_TOGGLE    2002
#define MENU_REFRESH_CONFIG  2003
#define MENU_EDIT_CONFIG     2004
#define MENU_REVEAL_CONFIG   2005
#define MENU_ABOUT           2006
#define MENU_CHECK_UPDATES   2007
#define MENU_QUIT            2008

// About dialog controls
#define ID_LINK              3001
#define ID_OK_BUTTON         3002

// Menu text constants
#define MENU_TEXT_PAUSE      "Pause"
#define MENU_TEXT_RESUME     "Resume"
#define MENU_TEXT_DEBUG_START "Start Debug Mode"
#define MENU_TEXT_DEBUG_STOP  "Stop Debug Mode"

// Buffer size for explorer.exe /select command formatting
#define EXPLORER_ARGS_PADDING 20

// Globals
static NOTIFYICONDATA tray_data = {0};
static HMENU menu = NULL;
static HWND window = NULL;
static UINT taskbar_restart_msg = 0;

// External functions and globals we'll use
extern void put_config_path(wchar_t * path);
extern int load_config_file(wchar_t * path);
extern void reset_config();
extern void create_console();
extern void destroy_console();
extern int g_paused;
extern int g_debug;

void update_menu_item(UINT menu_id, const char* text) {
    MENUITEMINFO menu_item_info = {0};
    menu_item_info.cbSize = sizeof(MENUITEMINFO);
    menu_item_info.fMask = MIIM_STRING;
    menu_item_info.dwTypeData = (char*)text;
    SetMenuItemInfo(menu, menu_id, FALSE, &menu_item_info);
}

void update_tray(void) {
    update_menu_item(MENU_PAUSE_RESUME, g_paused ? MENU_TEXT_RESUME : MENU_TEXT_PAUSE);
    sprintf(tray_data.szTip, "dual-key-remap%s", g_paused ? " (off)" : "");
    update_menu_item(MENU_DEBUG_TOGGLE, g_debug ? MENU_TEXT_DEBUG_STOP : MENU_TEXT_DEBUG_START);
    
    // Notify system tray of changes
    Shell_NotifyIcon(NIM_MODIFY, &tray_data);
}

void open_url(const wchar_t* url) {
    ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOW);
}

void check_updates() {
    open_url(L"https://github.com/ililim/dual-key-remap/releases/latest");
}

void open_about() {
    open_url(L"https://github.com/ililim/dual-key-remap");
}

void edit_config() {
    wchar_t config_path[MAX_PATH];
    put_config_path(config_path);
    ShellExecuteW(NULL, L"open", config_path, NULL, NULL, SW_SHOW);
}

void reveal_config() {
    wchar_t config_path[MAX_PATH];
    put_config_path(config_path);
    wchar_t explorer_args[MAX_PATH + EXPLORER_ARGS_PADDING];
    swprintf(explorer_args, sizeof(explorer_args)/sizeof(wchar_t), L"/select,\"%s\"", config_path);
    ShellExecuteW(NULL, L"open", L"explorer.exe", explorer_args, NULL, SW_SHOW);
}

void reload_config() {
    wchar_t config_path[MAX_PATH];
    put_config_path(config_path);
    create_console(); // load_config_file can print, so ensure console is available
    reset_config();
    int error = load_config_file(config_path);
    if (error) {
        MessageBox(NULL, "Failed to reload config file", "dual-key-remap config error", MB_ICONERROR);
    } else if (g_debug) {
        printf("Configuration reloaded from config file\n");
    }
    update_tray();
    if (!g_debug) destroy_console();
    else printf("-- DEBUG MODE --\n");
}

void toggle_pause() {
    g_paused = !g_paused;
    update_tray();
}

void toggle_debug() {
    g_debug = !g_debug;
    update_tray();
    if (g_debug) {
        create_console();
        printf("-- DEBUG MODE --\n");
    } else {
        destroy_console();
    }
}

void create_menu() {
    menu = CreatePopupMenu();

    AppendMenu(menu, MF_STRING, MENU_PAUSE_RESUME, MENU_TEXT_PAUSE);
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, MENU_REVEAL_CONFIG, "Show Config");
    AppendMenu(menu, MF_STRING, MENU_EDIT_CONFIG, "Edit Config");
    AppendMenu(menu, MF_STRING, MENU_REFRESH_CONFIG, "Reload Config");
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, MENU_DEBUG_TOGGLE, g_debug ? MENU_TEXT_DEBUG_STOP : MENU_TEXT_DEBUG_START);
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, MENU_ABOUT, "About");
    AppendMenu(menu, MF_STRING, MENU_CHECK_UPDATES, "Check Updates");
    AppendMenu(menu, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu, MF_STRING, MENU_QUIT, "Exit");
}

void show_menu() {
    POINT pos;
    GetCursorPos(&pos);
    SetForegroundWindow(window);
    TrackPopupMenu(menu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pos.x, pos.y, 0, window, NULL);
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
        case WM_TRAY_ICON_MESSAGE:
            if (lparam == WM_RBUTTONUP || lparam == WM_LBUTTONUP) {
                show_menu();
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case MENU_PAUSE_RESUME:   toggle_pause(); break;
                case MENU_DEBUG_TOGGLE:   toggle_debug(); break;
                case MENU_REFRESH_CONFIG: reload_config(); break;
                case MENU_EDIT_CONFIG:    edit_config(); break;
                case MENU_REVEAL_CONFIG:  reveal_config(); break;
                case MENU_ABOUT:          open_about(); break;
                case MENU_CHECK_UPDATES:  check_updates(); break;
                case MENU_QUIT:           PostQuitMessage(0); break;
            }
            break;

        default:
            // Handle taskbar recreation (when Explorer.exe restarts)
            if (message == taskbar_restart_msg) {
                // Re-add our tray icon since the taskbar was recreated
                Shell_NotifyIcon(NIM_ADD, &tray_data);
                return 0;
            }
            return DefWindowProc(hwnd, message, wparam, lparam);
    }
    return 0;
}

int init_tray_icon() {
    // Make app DPI-aware to avoid blurry text on high-DPI displays
    SetProcessDPIAware();

    // Register for taskbar recreation notification
    taskbar_restart_msg = RegisterWindowMessage(TEXT("TaskbarCreated"));

    // Register window class for tray messages
    WNDCLASS window_class = {0};
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = GetModuleHandle(NULL);
    window_class.lpszClassName = "dual-key-remap-tray";

    if (!RegisterClass(&window_class)) {
        return 0;
    }

    // Create invisible window for tray messages
    window = CreateWindow("dual-key-remap-tray", "dual-key-remap",
                               0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!window) {
        return 0;
    }

    // Create menu
    create_menu();

    // Setup tray icon
    tray_data.cbSize = sizeof(NOTIFYICONDATA);
    tray_data.hWnd = window;
    tray_data.uID = TRAY_ICON_ID;
    tray_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    tray_data.uCallbackMessage = WM_TRAY_ICON_MESSAGE;

    // Load icon - try embedded resource first, then file, then default
    // Uncomment these lines when resource.h is available:
    // tray_data.hIcon = (HICON)LoadImage(GetModuleHandle(NULL),
    //                                        MAKEINTRESOURCE(IDI_TRAY_ICON), IMAGE_ICON,
    //                                        GetSystemMetrics(SM_CXSMICON),
    //                                        GetSystemMetrics(SM_CYSMICON), 0);

    // For now, load from file app folder
    if (!tray_data.hIcon) {
        tray_data.hIcon = (HICON)LoadImage(NULL, "logo.ico", IMAGE_ICON,
                                               GetSystemMetrics(SM_CXSMICON),
                                               GetSystemMetrics(SM_CYSMICON),
                                               LR_LOADFROMFILE);
    }
    if (!tray_data.hIcon) { // Try 16x16 fallback
        tray_data.hIcon = (HICON)LoadImage(NULL, "logo.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    }
    if (!tray_data.hIcon) { // Fallback to default application icon
        tray_data.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }

    strcpy(tray_data.szTip, "dual-key-remap");

    // Add to system tray
    if (!Shell_NotifyIcon(NIM_ADD, &tray_data)) {
        return 0;
    }

    return 1;
}

void cleanup_tray_icon() {
    Shell_NotifyIcon(NIM_DELETE, &tray_data);
    if (menu) {
        DestroyMenu(menu);
    }
    if (window) {
        DestroyWindow(window);
    }
}
