#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_ERR_LEN 40

// A semi random value used to identify inputs generated
// by Dual Key Remap. Ideally high to minimize chances of a collision
// with a real pointer used by another application
#define INJECTED_KEY_ID 0xFFC3CED7

enum inputUpDown {
	INPUT_KEYDOWN,
	INPUT_KEYUP
};

// This flag needs to be high enough to be stored
// on top of key code values up to 0xFFFF
enum keyCodeType {
	HARDWARE_KEY = 0,
	VIRTUAL_KEY = 2 << 15,
};

struct keyDef {
	char *name;
	int code;
	int altCode;
};

enum remappedKeyState {
	NOT_HELD_DOWN,
	HELD_DOWN_ALONE,
	HELD_DOWN_WITH_OTHER
};

struct keyState {
    int remapKey;
    int altRemapKey;
    int whenAlone;
    int withOther;
	enum remappedKeyState state;

    struct keyState *next;
};

struct appState {
    int debug;
    struct keyState *keysHead;
    struct keyState *keysTail;
};

void addKeytoState(struct appState *state, int remapKey, int altRemapKey, int whenAlone, int withOther) {
    struct keyState *key = malloc(sizeof(struct keyState));
    key->remapKey = remapKey;
    key->altRemapKey = altRemapKey;
    key->whenAlone = whenAlone;
    key->withOther = withOther;
	key->state = NOT_HELD_DOWN;

    if (!state->keysHead) {
        state->keysHead = key;
    } else {
        state->keysTail->next = key;
    }
    state->keysTail = key;
}

// Globals
HHOOK g_keyboardHook;
HHOOK g_mouseHook;
struct appState g_state;

// The table of configurable key names and their respective codes.
// These code value can refer to either a virtual or harware code,
// as delimited by the VIRTUAL_KEY binary flag.
// Note: This means the caller will have to unset this flag to use the code.
//
// Generally using hardware scan codes is recommended as these do not
// get intercepted by DirectX and thus will work in more applications.
struct keyDef keytable[] = {
	// Multi Keys: These are keys that refer to two codes (left & right keys)
	{"CTRL", 0x001D, 0xE01D},
	{"SHIFT", 0x002A, 0x0036},
	{"ALT", 0x0038, 0xE038},
	{"WINDOWS", 0xE05B, 0xE05C},

	// Hardware scan code keys
	{"LEFT_CTRL", 0x001D},
	{"RIGHT_CTRL", 0xE01D},
	{"LEFT_SHIFT", 0x002A},
	{"RIGHT_SHIFT", 0x0036},
	{"LEFT_ALT", 0x0038},
	{"RIGHT_ALT", 0xE038},
	{"LEFT_WINDOWS", 0xE05B},
	{"RIGHT_WINDOWS", 0xE05C},

	{"ESCAPE", 0x01},
	{"SPACE", 0x39},
	{"CAPSLOCK", 0x3A},
	{"BACKSPACE", 0x0E},
	{"TAB", 0x0F},
	{"ENTER", 0x1C},

	{"Q", 0x0010},
	{"W", 0x0011},
	{"E", 0x0012},
	{"R", 0x0013},
	{"T", 0x0014},
	{"Y", 0x0015},
	{"U", 0x0016},
	{"I", 0x0017},
	{"O", 0x0018},
	{"P", 0x0019},
	{"D", 0x0020},
	{"F", 0x0021},
	{"G", 0x0022},
	{"H", 0x0023},
	{"J", 0x0024},
	{"K", 0x0025},
	{"L", 0x0026},
	{"Z", 0x002C},
	{"X", 0x002D},
	{"C", 0x002E},
	{"V", 0x002F},
	{"B", 0x0030},
	{"N", 0x0031},
	{"M", 0x0032},
	{"A", 0x001E},
	{"S", 0x001F},

	{"1", 0x0002},
	{"!", 0x0002},
	{"2", 0x0003},
	{"@", 0x0003},
	{"3", 0x0004},
	{"#", 0x0004},
	{"4", 0x0005},
	{"$", 0x0005},
	{"5", 0x0006},
	{"%", 0x0006},
	{"6", 0x0007},
	{"^", 0x0007},
	{"7", 0x0008},
	{"&", 0x0008},
	{"8", 0x0009},
	{"*", 0x0009},
	{"9", 0x000A},
	{"(", 0x000A},
	{"0", 0x000B},
	{")", 0x000B},
	{"- ", 0x000C},
	{"_", 0x000C},
	{"=", 0x000D},
	{"+", 0x000D},
	{"[", 0x001A},
	{"{", 0x001A},
	{"]", 0x001B},
	{"}", 0x001B},

	{";", 0x0027},
	{":", 0x0027},
	{"'", 0x0028},
	{"\"", 0x0028},
	{"`", 0x0029},
	{"~", 0x0029},
	{"\\", 0x002B},
	{"|", 0x002B},

	{",", 0x0033},
	{"<", 0x0033},
	{".", 0x0034},
	{">", 0x0034},
	{"/", 0x0035},
	{"?", 0x0035},
	{"<>|", 0xE056},

	{"F1", 0x003B},
	{"F2", 0x003C},
	{"F3", 0x003D},
	{"F4", 0x003E},
	{"F5", 0x003F},
	{"F6", 0x0040},
	{"F7", 0x0041},
	{"F8", 0x0042},
	{"F9", 0x0043},
	{"F10", 0x0044},
	{"F11", 0x0057},
	{"F12", 0x0058},
	{"F13", 0x0064},
	{"F14", 0x0065},
	{"F15", 0x0066},
	{"F16", 0x0067},
	{"F17", 0x0068},
	{"F18", 0x0069},
	{"F19", 0x006A},
	{"F20", 0x006B},
	{"F21", 0x006C},
	{"F22", 0x006D},
	{"F23", 0x006E},
	{"F24", 0x006F},

	{"UP", 0xE048},
	{"LEFT", 0xE04B},
	{"RIGHT", 0xE04D},
	{"DOWN", 0xE050},

	{"HOME", 0xE047},
	{"PAGEUP", 0xE049},
	{"PAGEDOWN", 0xE051},
	{"END", 0xE04F},
	{"INSERT", 0xE052},
	{"DELETE", 0xE053},

	{"NUMLOCK", 0x0045},
	{"SCROLLLOCK", 0x0046},
	{"PRTSC", 0xE037},

	{"NUM_*", 0x0037},
	{"NUM_7", 0x0047},
	{"NUM_8", 0x0048},
	{"NUM_9", 0x0049},
	{"NUM_-", 0x004A},
	{"NUM_4", 0x004B},
	{"NUM_5", 0x004C},
	{"NUM_6", 0x004D},
	{"NUM_+", 0x004E},
	{"NUM_1", 0x004F},
	{"NUM_2", 0x0050},
	{"NUM_3", 0x0051},
	{"NUM_0", 0x0052},
	{"NUM_.", 0x0053},
	{"NUM_Enter", 0xE01C},
	{"NUM_/", 0xE035},

	// Virtual code keys
	{"VK_0",                   0x30 | VIRTUAL_KEY},
	{"VK_1",                   0x31 | VIRTUAL_KEY},
	{"VK_2",                   0x32 | VIRTUAL_KEY},
	{"VK_3",                   0x33 | VIRTUAL_KEY},
	{"VK_4",                   0x34 | VIRTUAL_KEY},
	{"VK_5",                   0x35 | VIRTUAL_KEY},
	{"VK_6",                   0x36 | VIRTUAL_KEY},
	{"VK_7",                   0x37 | VIRTUAL_KEY},
	{"VK_8",                   0x38 | VIRTUAL_KEY},
	{"VK_9",                   0x39 | VIRTUAL_KEY},
	{"VK_A",                   0x41 | VIRTUAL_KEY},
	{"VK_B",                   0x42 | VIRTUAL_KEY},
	{"VK_C",                   0x43 | VIRTUAL_KEY},
	{"VK_D",                   0x44 | VIRTUAL_KEY},
	{"VK_E",                   0x45 | VIRTUAL_KEY},
	{"VK_F",                   0x46 | VIRTUAL_KEY},
	{"VK_G",                   0x47 | VIRTUAL_KEY},
	{"VK_H",                   0x48 | VIRTUAL_KEY},
	{"VK_I",                   0x49 | VIRTUAL_KEY},
	{"VK_J",                   0x4A | VIRTUAL_KEY},
	{"VK_K",                   0x4B | VIRTUAL_KEY},
	{"VK_L",                   0x4C | VIRTUAL_KEY},
	{"VK_M",                   0x4D | VIRTUAL_KEY},
	{"VK_N",                   0x4E | VIRTUAL_KEY},
	{"VK_O",                   0x4F | VIRTUAL_KEY},
	{"VK_P",                   0x50 | VIRTUAL_KEY},
	{"VK_Q",                   0x51 | VIRTUAL_KEY},
	{"VK_R",                   0x52 | VIRTUAL_KEY},
	{"VK_S",                   0x53 | VIRTUAL_KEY},
	{"VK_T",                   0x54 | VIRTUAL_KEY},
	{"VK_U",                   0x55 | VIRTUAL_KEY},
	{"VK_V",                   0x56 | VIRTUAL_KEY},
	{"VK_W",                   0x57 | VIRTUAL_KEY},
	{"VK_X",                   0x58 | VIRTUAL_KEY},
	{"VK_Y",                   0x59 | VIRTUAL_KEY},
	{"VK_Z",                   0x5A | VIRTUAL_KEY},
	{"VK_LBUTTON",             0x01 | VIRTUAL_KEY}, // Left mouse button
	{"VK_RBUTTON",             0x02 | VIRTUAL_KEY}, // Right mouse button
	{"VK_CANCEL",              0x03 | VIRTUAL_KEY}, // Control-break processing
	{"VK_MBUTTON",             0x04 | VIRTUAL_KEY}, // Middle mouse button (three-button mouse)
	{"VK_XBUTTON1",            0x05 | VIRTUAL_KEY}, // X1 mouse button
	{"VK_XBUTTON2",            0x06 | VIRTUAL_KEY}, // X2 mouse button
	{"VK_BACK",                0x08 | VIRTUAL_KEY}, // BACKSPACE key
	{"VK_TAB",                 0x09 | VIRTUAL_KEY}, // TAB key
	{"VK_CLEAR",               0x0C | VIRTUAL_KEY}, // CLEAR key
	{"VK_RETURN",              0x0D | VIRTUAL_KEY}, // ENTER key
	{"VK_SHIFT",               0x10 | VIRTUAL_KEY}, // SHIFT key
	{"VK_CONTROL",             0x11 | VIRTUAL_KEY}, // CTRL key
	{"VK_MENU",                0x12 | VIRTUAL_KEY}, // ALT key
	{"VK_PAUSE",               0x13 | VIRTUAL_KEY}, // PAUSE key
	{"VK_CAPITAL",             0x14 | VIRTUAL_KEY}, // CAPS LOCK key
	{"VK_KANA",                0x15 | VIRTUAL_KEY}, // IME Kana mode
	{"VK_HANGUEL",             0x15 | VIRTUAL_KEY}, // IME Hanguel mode
	{"VK_JUNJA",               0x17 | VIRTUAL_KEY}, // IME Junja mode
	{"VK_FINAL",               0x18 | VIRTUAL_KEY}, // IME final mode
	{"VK_HANJA",               0x19 | VIRTUAL_KEY}, // IME Hanja mode
	{"VK_KANJI",               0x19 | VIRTUAL_KEY}, // IME Kanji mode
	{"VK_ESCAPE",              0x1B | VIRTUAL_KEY}, // ESC key
	{"VK_CONVERT",             0x1C | VIRTUAL_KEY}, // IME convert
	{"VK_NONCONVERT",          0x1D | VIRTUAL_KEY}, // IME nonconvert
	{"VK_ACCEPT",              0x1E | VIRTUAL_KEY}, // IME accept
	{"VK_MODECHANGE",          0x1F | VIRTUAL_KEY}, // IME mode change request
	{"VK_SPACE",               0x20 | VIRTUAL_KEY}, // SPACEBAR
	{"VK_PRIOR",               0x21 | VIRTUAL_KEY}, // PAGE UP key
	{"VK_NEXT",                0x22 | VIRTUAL_KEY}, // PAGE DOWN key
	{"VK_END",                 0x23 | VIRTUAL_KEY}, // END key
	{"VK_HOME",                0x24 | VIRTUAL_KEY}, // HOME key
	{"VK_LEFT",                0x25 | VIRTUAL_KEY}, // LEFT ARROW key
	{"VK_UP",                  0x26 | VIRTUAL_KEY}, // UP ARROW key
	{"VK_RIGHT",               0x27 | VIRTUAL_KEY}, // RIGHT ARROW key
	{"VK_DOWN",                0x28 | VIRTUAL_KEY}, // DOWN ARROW key
	{"VK_SELECT",              0x29 | VIRTUAL_KEY}, // SELECT key
	{"VK_PRINT",               0x2A | VIRTUAL_KEY}, // PRINT key
	{"VK_EXECUTE",             0x2B | VIRTUAL_KEY}, // EXECUTE key
	{"VK_SNAPSHOT",            0x2C | VIRTUAL_KEY}, // PRINT SCREEN key
	{"VK_INSERT",              0x2D | VIRTUAL_KEY}, // INS key
	{"VK_DELETE",              0x2E | VIRTUAL_KEY}, // DEL key
	{"VK_HELP",                0x2F | VIRTUAL_KEY}, // HELP key
	{"VK_LWIN",                0x5B | VIRTUAL_KEY}, // Left Windows key (Natural keyboard)
	{"VK_RWIN",                0x5C | VIRTUAL_KEY}, // Right Windows key (Natural keyboard)
	{"VK_APPS",                0x5D | VIRTUAL_KEY}, // Applications key (Natural keyboard)
	{"VK_SLEEP",               0x5F | VIRTUAL_KEY}, // Computer Sleep key
	{"VK_NUMPAD0",             0x60 | VIRTUAL_KEY}, // Numeric keypad 0 key
	{"VK_NUMPAD1",             0x61 | VIRTUAL_KEY}, // Numeric keypad 1 key
	{"VK_NUMPAD2",             0x62 | VIRTUAL_KEY}, // Numeric keypad 2 key
	{"VK_NUMPAD3",             0x63 | VIRTUAL_KEY}, // Numeric keypad 3 key
	{"VK_NUMPAD4",             0x64 | VIRTUAL_KEY}, // Numeric keypad 4 key
	{"VK_NUMPAD5",             0x65 | VIRTUAL_KEY}, // Numeric keypad 5 key
	{"VK_NUMPAD6",             0x66 | VIRTUAL_KEY}, // Numeric keypad 6 key
	{"VK_NUMPAD7",             0x67 | VIRTUAL_KEY}, // Numeric keypad 7 key
	{"VK_NUMPAD8",             0x68 | VIRTUAL_KEY}, // Numeric keypad 8 key
	{"VK_NUMPAD9",             0x69 | VIRTUAL_KEY}, // Numeric keypad 9 key
	{"VK_MULTIPLY",            0x6A | VIRTUAL_KEY}, // Multiply key
	{"VK_ADD",                 0x6B | VIRTUAL_KEY}, // Add key
	{"VK_SEPARATOR",           0x6C | VIRTUAL_KEY}, // Separator key
	{"VK_SUBTRACT",            0x6D | VIRTUAL_KEY}, // Subtract key
	{"VK_DECIMAL",             0x6E | VIRTUAL_KEY}, // Decimal key
	{"VK_DIVIDE",              0x6F | VIRTUAL_KEY}, // Divide key
	{"VK_F1",                  0x70 | VIRTUAL_KEY}, // F1 key
	{"VK_F2",                  0x71 | VIRTUAL_KEY}, // F2 key
	{"VK_F3",                  0x72 | VIRTUAL_KEY}, // F3 key
	{"VK_F4",                  0x73 | VIRTUAL_KEY}, // F4 key
	{"VK_F5",                  0x74 | VIRTUAL_KEY}, // F5 key
	{"VK_F6",                  0x75 | VIRTUAL_KEY}, // F6 key
	{"VK_F7",                  0x76 | VIRTUAL_KEY}, // F7 key
	{"VK_F8",                  0x77 | VIRTUAL_KEY}, // F8 key
	{"VK_F9",                  0x78 | VIRTUAL_KEY}, // F9 key
	{"VK_F10",                 0x79 | VIRTUAL_KEY}, // F10 key
	{"VK_F11",                 0x7A | VIRTUAL_KEY}, // F11 key
	{"VK_F12",                 0x7B | VIRTUAL_KEY}, // F12 key
	{"VK_F13",                 0x7C | VIRTUAL_KEY}, // F13 key
	{"VK_F14",                 0x7D | VIRTUAL_KEY}, // F14 key
	{"VK_F15",                 0x7E | VIRTUAL_KEY}, // F15 key
	{"VK_F16",                 0x7F | VIRTUAL_KEY}, // F16 key
	{"VK_F17",                 0x80 | VIRTUAL_KEY}, // F17 key
	{"VK_F18",                 0x81 | VIRTUAL_KEY}, // F18 key
	{"VK_F19",                 0x82 | VIRTUAL_KEY}, // F19 key
	{"VK_F20",                 0x83 | VIRTUAL_KEY}, // F20 key
	{"VK_F21",                 0x84 | VIRTUAL_KEY}, // F21 key
	{"VK_F22",                 0x85 | VIRTUAL_KEY}, // F22 key
	{"VK_F23",                 0x86 | VIRTUAL_KEY}, // F23 key
	{"VK_F24",                 0x87 | VIRTUAL_KEY}, // F24 key
	{"VK_NUMLOCK",             0x90 | VIRTUAL_KEY}, // NUM LOCK key
	{"VK_SCROLL",              0x91 | VIRTUAL_KEY}, // SCROLL LOCK key
	{"VK_LSHIFT",              0xA0 | VIRTUAL_KEY}, // Left SHIFT key
	{"VK_RSHIFT",              0xA1 | VIRTUAL_KEY}, // Right SHIFT key
	{"VK_LCONTROL",            0xA2 | VIRTUAL_KEY}, // Left CONTROL key
	{"VK_RCONTROL",            0xA3 | VIRTUAL_KEY}, // Right CONTROL key
	{"VK_LMENU",               0xA4 | VIRTUAL_KEY}, // Left MENU key
	{"VK_RMENU",               0xA5 | VIRTUAL_KEY}, // Right MENU key
	{"VK_BROWSER_BACK",        0xA6 | VIRTUAL_KEY}, // Browser Back key
	{"VK_BROWSER_FORWARD",     0xA7 | VIRTUAL_KEY}, // Browser Forward key
	{"VK_BROWSER_REFRESH",     0xA8 | VIRTUAL_KEY}, // Browser Refresh key
	{"VK_BROWSER_STOP",        0xA9 | VIRTUAL_KEY}, // Browser Stop key
	{"VK_BROWSER_SEARCH",      0xAA | VIRTUAL_KEY}, // Browser Search key
	{"VK_BROWSER_FAVORITES",   0xAB | VIRTUAL_KEY}, // Browser Favorites key
	{"VK_BROWSER_HOME",        0xAC | VIRTUAL_KEY}, // Browser Start and Home key
	{"VK_VOLUME_MUTE",         0xAD | VIRTUAL_KEY}, // Volume Mute key
	{"VK_VOLUME_DOWN",         0xAE | VIRTUAL_KEY}, // Volume Down key
	{"VK_VOLUME_UP",           0xAF | VIRTUAL_KEY}, // Volume Up key
	{"VK_MEDIA_NEXT_TRACK",    0xB0 | VIRTUAL_KEY}, // Next Track key
	{"VK_MEDIA_PREV_TRACK",    0xB1 | VIRTUAL_KEY}, // Previous Track key
	{"VK_MEDIA_STOP",          0xB2 | VIRTUAL_KEY}, // Stop Media key
	{"VK_MEDIA_PLAY_PAUSE",    0xB3 | VIRTUAL_KEY}, // Play/Pause Media key
	{"VK_LAUNCH_MAIL",         0xB4 | VIRTUAL_KEY}, // Start Mail key
	{"VK_LAUNCH_MEDIA_SELECT", 0xB5 | VIRTUAL_KEY}, // Select Media key
	{"VK_LAUNCH_APP1",         0xB6 | VIRTUAL_KEY}, // Start Application 1 key
	{"VK_LAUNCH_APP2",         0xB7 | VIRTUAL_KEY}, // Start Application 2 key
	{"VK_OEM_1",               0xBA | VIRTUAL_KEY}, // Used for miscellaneous characters; it can vary by keyboard.
	{"VK_OEM_PLUS",            0xBB | VIRTUAL_KEY}, // For any country/region, the '+' key
	{"VK_OEM_COMMA",           0xBC | VIRTUAL_KEY}, // For any country/region, the ',' key
	{"VK_OEM_MINUS",           0xBD | VIRTUAL_KEY}, // For any country/region, the '-' key
	{"VK_OEM_PERIOD",          0xBE | VIRTUAL_KEY}, // For any country/region, the '.' key
	{"VK_OEM_2",               0xBF | VIRTUAL_KEY}, // Used for miscellaneous characters; it can vary by keyboard
	{"VK_OEM_3",               0xC0 | VIRTUAL_KEY}, // Used for miscellaneous characters; it can vary by keyboard
	{"VK_OEM_4",               0xDB | VIRTUAL_KEY}, // Used for miscellaneous characters; it can vary by keyboard
	{"VK_OEM_5",               0xDC | VIRTUAL_KEY}, // Used for miscellaneous characters; it can vary by keyboard
	{"VK_OEM_6",               0xDD | VIRTUAL_KEY}, // Used for miscellaneous characters; it can vary by keyboard
	{"VK_OEM_7",               0xDE | VIRTUAL_KEY}, // Used for miscellaneous characters; it can vary by keyboard
	{"VK_OEM_8",               0xDF | VIRTUAL_KEY}, // Used for miscellaneous characters; it can vary by keyboard
	{"VK_OEM_102",             0xE2 | VIRTUAL_KEY}, // Either the angle bracket key or the backslash key on the RT 102-key keyboard
	{"VK_PROCESSKEY",          0xE5 | VIRTUAL_KEY}, // IME PROCESS key
	{"VK_PACKET",              0xE7 | VIRTUAL_KEY}, // Used to pass Unicode characters as if they were keystrokes
	{"VK_ATTN",                0xF6 | VIRTUAL_KEY}, // Attn key
	{"VK_CRSEL",               0xF7 | VIRTUAL_KEY}, // CrSel key
	{"VK_EXSEL",               0xF8 | VIRTUAL_KEY}, // ExSel key
	{"VK_EREOF",               0xF9 | VIRTUAL_KEY}, // Erase EOF key
	{"VK_PLAY",                0xFA | VIRTUAL_KEY}, // Play key
	{"VK_ZOOM",                0xFB | VIRTUAL_KEY}, // Zoom key
	{"VK_NONAME",              0xFC | VIRTUAL_KEY}, // Reserved
	{"VK_PA1",                 0xFD | VIRTUAL_KEY}, // PA1 key
	{"VK_OEM_CLEAR",           0xFE | VIRTUAL_KEY}, // Clear key
};
#define KEY_TABLE_LEN (sizeof(keytable)/sizeof(struct keyDef))

struct keyDef *keyDefByName(char *name)
{
	if (!name) return NULL;
	for (int i = 0; i < KEY_TABLE_LEN; ++i)
	{
		struct keyDef *key = keytable + i;
		if (strcmp(key->name, name) == 0)
			return key;
	}
	return NULL;
}

void trimnewline(char* buffer)
{
	buffer[strcspn(buffer, "\r\n")] = 0;
}

int setStateFromConfigLine(struct appState *state, char *line, int linenum)
{
	// Handle debug mode
	if (strstr(line, "debug=1"))
	{
		state->debug = 1;
		return 0;
	}
	else if (strstr(line, "debug=0"))
	{
		state->debug = 0;
		return 0;
	}

	// Handle key remappings
	char *keyName = strchr(line, '=') + 1;
	struct keyDef *keyDef = keyDefByName(keyName);
	struct keyState *keyState = state->keysTail;

	if (!keyDef)
	{
		printf("Cannot parse line %i in config: Unknown key '%s'.\nMake sure you've used the correct keycode, and that that there is no extraneous whitespace.\n", linenum, keyName);
		return 1;
	}

	if (strstr(line, "remap_key="))
	{
		addKeytoState(state, keyDef->code, keyDef->altCode, 0, 0);
	}
	else if (strstr(line, "when_alone="))
	{
		state->keysTail->whenAlone = keyDef->code;
	}
	else if (strstr(line, "with_other="))
	{
		state->keysTail->withOther = keyDef->code;
	}
	else
	{
		printf("Cannot parse line %i in config: '%s'.\nMake sure you've used a valid config option, and that that there is no extraneous whitespace. Supported options: 'debug', 'remap_key', 'when_alone', 'with_other'.\n", linenum, line);
		return 1;
	}

	return 0;
}

int initStateFromConfig(struct appState *state, char *path)
{
	FILE *fs;
	char line[40];

    if (fopen_s(&fs, path, "r") > 0)
	{
		printf("Cannot open configuration file '%s'. Make sure it is in the same directory as 'key-dual-remap.exe'.\n", path);
        return 1;
	}

    int linenum = 1;
	while (fgets(line, 40, fs))
	{
		trimnewline(line);
		if (line[0] == '\0') continue; // Ignore empty lines
		if (setStateFromConfigLine(state, line, ++linenum)) {
			fclose(fs);
			return 1;
		}
	};
	fclose(fs);

	// Validate remappings
	struct keyState *key = g_state.keysHead;
	do {
		if (!(key && key->whenAlone && key->withOther)) {
			printf("Invalid config: Incomplete remapping.\nEach remapping must have 'remap_key', 'when_alone', and 'with_other' declared.\n");
			return 1;
		}
        key = key->next;
    } while(key);

    return 0;
}

void sendKeyEvent(int keyCode, int keyUpDown)
{
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.time = 0;
	input.ki.dwExtraInfo = (ULONG_PTR)INJECTED_KEY_ID;

	if (keyCode & VIRTUAL_KEY) {
		input.ki.wScan = 0;
		input.ki.wVk = keyCode & ~VIRTUAL_KEY;
		input.ki.dwFlags = keyUpDown == INPUT_KEYUP ? KEYEVENTF_KEYUP : 0;
	}
	else // Hardware key
	{
		input.ki.wScan = keyCode;
		input.ki.wVk = 0;
		input.ki.dwFlags = keyUpDown == INPUT_KEYUP ? KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP : KEYEVENTF_SCANCODE;
	}

	SendInput(1, &input, sizeof(INPUT));
}

int keyCodeMatchesInput(int keyCode, const KBDLLHOOKSTRUCT *key)
{
	return (keyCode & VIRTUAL_KEY)
		? (keyCode & ~VIRTUAL_KEY) == key->vkCode
		: keyCode == key->scanCode;
}

LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// Required per Microsoft documentation
	if (nCode != HC_ACTION) return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
	
	// If mouse is pressed down we update the held down keys to with_other
	switch (wParam) {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_NCXBUTTONDOWN:
		case WM_XBUTTONDOWN:
			struct keyState *keyState = g_state.keysHead;
			while(keyState) {
				if (keyState->state == HELD_DOWN_ALONE) {
					keyState->state = HELD_DOWN_WITH_OTHER;
					sendKeyEvent(keyState->withOther, INPUT_KEYDOWN);
				}
				keyState = keyState->next;
			}
	}
	return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// Required per Microsoft documentation
	if (nCode != HC_ACTION) return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);


	const KBDLLHOOKSTRUCT *inputKey = (KBDLLHOOKSTRUCT *) lParam;
	enum inputUpDown inputUpDown = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) ? INPUT_KEYUP : INPUT_KEYDOWN;

	// Check if this is one of the keys we are looking for
	int isRemappedKey = 0;
    struct keyState *keyState = g_state.keysHead;
	while(keyState)
    {
		if (
			keyCodeMatchesInput(keyState->remapKey, inputKey) ||
			(keyState->altRemapKey && keyCodeMatchesInput(keyState->altRemapKey, inputKey))
		) {
			isRemappedKey = 1;
			break;
		}
        keyState = keyState->next;
    }

	if (g_state.debug)
	{
		printf("Logged keypress (Injected: %i, vkCode: %lu, scanCode: %lu, flags: %lu, dwExtraInfo: %lu\n",
			((inputKey->flags & LLKHF_INJECTED) == LLKHF_INJECTED), inputKey->vkCode, inputKey->scanCode, inputKey->flags, inputKey->dwExtraInfo);
	}

	// Handles non-remapped keys:
	// This includes injected inputs to avoid recursive loops
	if (!isRemappedKey || inputKey->dwExtraInfo == INJECTED_KEY_ID)
	{
		// If we are pressing a key, we must update the state of all held down remapped keys
		if (inputUpDown == INPUT_KEYDOWN) {
			struct keyState *keyState = g_state.keysHead;
			while(keyState) {
				if (keyState->state == HELD_DOWN_ALONE) {
					keyState->state = HELD_DOWN_WITH_OTHER;
					sendKeyEvent(keyState->withOther, INPUT_KEYDOWN);
				}
				keyState = keyState->next;
			}
		}

		// Exit early, allowing others to process the key
		return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
	}

	// After this point the key is a remapped that is going to be swallowed

	// Handles remapped key KEYDOWN:
	// - Start listening other key presses
	// - Ignores KEYDOWN if according to state we're already holding down
	//   (possible with multiple keys/keyboards)
	if (inputUpDown == INPUT_KEYDOWN && keyState->state == NOT_HELD_DOWN)
	{
		keyState->state = HELD_DOWN_ALONE;
	}
	// Handles both cases of remapped key KEYUP:
	// - Either send whenAlone or finish sending withOther key
	// - Ignores KEYUP if according to state we're not holding down (multiple keys/keyboards)
	//   As a result, for multiple keys/keyboards only the first KEYUP will send output
	// Note: For safety we adjust our state _before_ sending further key inputs
	else if (inputUpDown == INPUT_KEYUP && keyState->state == HELD_DOWN_ALONE)
	{
		keyState->state = NOT_HELD_DOWN;
		sendKeyEvent(keyState->whenAlone, INPUT_KEYDOWN);
		sendKeyEvent(keyState->whenAlone, INPUT_KEYUP);
	}
	else if (inputUpDown == INPUT_KEYUP && keyState->state == HELD_DOWN_WITH_OTHER)
	{
		keyState->state = NOT_HELD_DOWN;
		// Keydown has already been sent by the other key callback
		sendKeyEvent(keyState->withOther, INPUT_KEYUP);
	}

	return 1;
}

int main(void)
{
	HWND hWnd = GetConsoleWindow();
	MSG msg;
	HANDLE hMutexHandle = CreateMutex(NULL, TRUE, "dual-key-remap.single-instance");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		printf("dual-key-remap.exe is already running!\n");
		goto end;
	}

	int err = initStateFromConfig(&g_state, "config.txt");
	if (err) {
		goto end;
	}

	g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, NULL, 0);
	g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, NULL, 0);
	if (g_keyboardHook == NULL)
	{
		printf("Cannot hook into the Windows API.\n");
		goto end;
	}

	// No errors, hide the console window if we're not debugging
	if (!g_state.debug) {
		ShowWindow(hWnd, SW_HIDE);
	}

	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	end:
		ReleaseMutex(hMutexHandle);
		CloseHandle(hMutexHandle);
		ShowWindow(hWnd, SW_SHOW);
		printf("\nPress any key to exit...\n");
		getch();
		return 1;
}
