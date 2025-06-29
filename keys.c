#include <stddef.h>
#include <string.h>

#ifndef KEYS_C
#define KEYS_C

struct KeyDef {
    char * name;
    int scan_code;
    int virt_code;
};
typedef struct KeyDef KEY_DEF;

#define VK_LEFT_CTRL 0xA2
#define VK_RIGHT_CTRL 0xA3
#define VK_LEFT_ALT 0xA4
#define VK_RIGHT_ALT 0xA5
#define VK_LEFT_SHIFT 0xA0
#define VK_RIGHT_SHIFT 0xA1
#define VK_LEFT_WIN 0x5B
#define VK_RIGHT_WIN 0x5C

#define VK_BACKSPACE 0x08
#define VK_CAPSLOCK 0x14
#define VK_ENTER 0x0D
#define VK_ESCAPE 0x1B
#define VK_SPACE 0x20
#define VK_TAB 0x09

#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28

#define VK_LBRACKET 0xDB
#define VK_RBRACKET 0xDD
#define VK_BACKSLASH 0xDC
#define VK_APOSTROPHE 0xDE

#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B
#define VK_F13 0x7C
#define VK_F14 0x7D
#define VK_F15 0x7E
#define VK_F16 0x7F
#define VK_F17 0x80
#define VK_F18 0x81
#define VK_F19 0x82
#define VK_F20 0x83
#define VK_F21 0x84
#define VK_F22 0x85
#define VK_F23 0x86

#define VK_KEY_0 0x30
#define VK_KEY_1 0x31
#define VK_KEY_2 0x32
#define VK_KEY_3 0x33
#define VK_KEY_4 0x34
#define VK_KEY_5 0x35
#define VK_KEY_6 0x36
#define VK_KEY_7 0x37
#define VK_KEY_8 0x38
#define VK_KEY_9 0x39

#define VK_KEY_A 0x41
#define VK_KEY_B 0x42
#define VK_KEY_C 0x43
#define VK_KEY_D 0x44
#define VK_KEY_E 0x45
#define VK_KEY_F 0x46
#define VK_KEY_G 0x47
#define VK_KEY_H 0x48
#define VK_KEY_I 0x49
#define VK_KEY_J 0x4A
#define VK_KEY_K 0x4B
#define VK_KEY_L 0x4C
#define VK_KEY_M 0x4D
#define VK_KEY_N 0x4E
#define VK_KEY_O 0x4F
#define VK_KEY_P 0x50
#define VK_KEY_Q 0x51
#define VK_KEY_R 0x52
#define VK_KEY_S 0x53
#define VK_KEY_T 0x54
#define VK_KEY_U 0x55
#define VK_KEY_V 0x56
#define VK_KEY_W 0x57
#define VK_KEY_X 0x58
#define VK_KEY_Y 0x59
#define VK_KEY_Z 0x5A

#define VK_NUM0 0x60
#define VK_NUM1 0x61
#define VK_NUM2 0x62
#define VK_NUM3 0x63
#define VK_NUM4 0x64
#define VK_NUM5 0x65
#define VK_NUM6 0x66
#define VK_NUM7 0x67
#define VK_NUM8 0x68
#define VK_NUM9 0x69
#define VK_NUM_MULTIPLY 0x6A
#define VK_NUM_ADD 0x6B
#define VK_NUM_SUBTRACT 0x6D
#define VK_NUM_DECIMAL 0x6E
#define VK_NUM_DIVIDE 0x6F

#define VK_APPS 0x5D
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_HOME 0x24
#define VK_END 0x23
#define VK_PAGE_UP 0x21
#define VK_PAGE_DOWN 0x22

#define VK_PRINT_SCREEN 0x2C
#define VK_NUMLOCK 0x90
#define VK_SCROLLLOCK 0x91
#define VK_PAUSE 0x13

#define VK_MEDIA_PLAY_PAUSE 0xB3
#define VK_MEDIA_NEXT_TRACK  0xB0
#define VK_MEDIA_PREV_TRACK  0xB1
#define VK_MEDIA_STOP        0xB2
#define VK_VOLUME_MUTE       0xAD
#define VK_VOLUME_DOWN       0xAE
#define VK_VOLUME_UP         0xAF

#define VK_PLUS 0xBB
#define VK_COMMA 0xBC
#define VK_MINUS 0xBD
#define VK_PERIOD 0xBE

#define VK_US_SEMI 0xBA // ;: key on US keyboards
#define VK_US_SLASH 0xBF // /? key on US keyboards
#define VK_US_TILDE 0xC0 // `~ key on US keyboards

#define MOUSE_DUMMY_VK 0xFF

#define SK_LEFT_CTRL 0x001D
#define SK_RIGHT_CTRL 0xE01D
#define SK_LEFT_ALT 0x0038
#define SK_RIGHT_ALT 0xE038
#define SK_LEFT_SHIFT 0x002A
#define SK_RIGHT_SHIFT 0x0036
#define SK_LEFT_WIN 0xE05B
#define SK_RIGHT_WIN 0xE05C

#define SK_BACKSPACE 0x0E
#define SK_CAPSLOCK 0x3A
#define SK_ENTER 0x1C
#define SK_ESCAPE 0x01
#define SK_SPACE 0x39
#define SK_TAB 0x0F

#define SK_UP 0X48
#define SK_LEFT 0X4B
#define SK_RIGHT 0X4D
#define SK_DOWN 0X50

#define SK_LBRACKET 0x1A
#define SK_RBRACKET 0x1B
#define SK_BACKSLASH 0x2B
#define SK_APOSTROPHE 0x28

#define SK_F1 0x3B
#define SK_F2 0x3C
#define SK_F3 0x3D
#define SK_F4 0x3E
#define SK_F5 0x3F
#define SK_F6 0x40
#define SK_F7 0x41
#define SK_F8 0x42
#define SK_F9 0x43
#define SK_F10 0x44
#define SK_F11 0x57
#define SK_F12 0x58
#define SK_F13 0x64
#define SK_F14 0x65
#define SK_F15 0x66
#define SK_F16 0x67
#define SK_F17 0x68
#define SK_F18 0x69
#define SK_F19 0x6A
#define SK_F20 0x6B
#define SK_F21 0x6C
#define SK_F22 0x6D
#define SK_F23 0x6E

#define SK_KEY_0 0x0B
#define SK_KEY_1 0x02
#define SK_KEY_2 0x03
#define SK_KEY_3 0x04
#define SK_KEY_4 0x05
#define SK_KEY_5 0x06
#define SK_KEY_6 0x07
#define SK_KEY_7 0x08
#define SK_KEY_8 0x09
#define SK_KEY_9 0x0A

#define SK_KEY_A 0x1E
#define SK_KEY_B 0x30
#define SK_KEY_C 0x2E
#define SK_KEY_D 0x20
#define SK_KEY_E 0x12
#define SK_KEY_F 0x21
#define SK_KEY_G 0x22
#define SK_KEY_H 0x23
#define SK_KEY_I 0x17
#define SK_KEY_J 0x24
#define SK_KEY_K 0x25
#define SK_KEY_L 0x26
#define SK_KEY_M 0x32
#define SK_KEY_N 0x31
#define SK_KEY_O 0x18
#define SK_KEY_P 0x19
#define SK_KEY_Q 0x10
#define SK_KEY_R 0x13
#define SK_KEY_S 0x1F
#define SK_KEY_T 0x14
#define SK_KEY_U 0x16
#define SK_KEY_V 0x2F
#define SK_KEY_W 0x11
#define SK_KEY_X 0x2D
#define SK_KEY_Y 0x15
#define SK_KEY_Z 0x2C

#define SK_NUM0 0x52
#define SK_NUM1 0x4F
#define SK_NUM2 0x50
#define SK_NUM3 0x51
#define SK_NUM4 0x4B
#define SK_NUM5 0x4C
#define SK_NUM6 0x4D
#define SK_NUM7 0x47
#define SK_NUM8 0x48
#define SK_NUM9 0x49
#define SK_NUM_MULTIPLY 0x37
#define SK_NUM_ADD 0x4E
#define SK_NUM_SUBTRACT 0x4A
#define SK_NUM_DECIMAL 0x53
#define SK_NUM_DIVIDE 0xE035

#define SK_APPS 0xE05D
#define SK_INSERT 0x52
#define SK_DELETE 0x53
#define SK_HOME 0x47
#define SK_END 0x4f
#define SK_PAGE_UP 0x49
#define SK_PAGE_DOWN 0x51

#define SK_PRINT_SCREEN 0x154
#define SK_NUMLOCK 0x145
#define SK_SCROLLLOCK 0x46
#define SK_PAUSE 0x45

#define SK_MEDIA_PLAY_PAUSE 0xE022
#define SK_MEDIA_NEXT_TRACK 0xE019
#define SK_MEDIA_PREV_TRACK 0xE010
#define SK_MEDIA_STOP 0xE024
#define SK_VOLUME_MUTE 0xE020
#define SK_VOLUME_DOWN 0xE02E
#define SK_VOLUME_UP 0xE030

#define SK_PLUS 0x0d
#define SK_COMMA 0x33
#define SK_MINUS 0x0c
#define SK_PERIOD 0x34

#define SK_US_SEMI 0x27  // ;: key on US keyboards
#define SK_US_SLASH 0x35 // /? key on US keyboards
#define SK_US_TILDE 0x29 // `~ key on US keyboards

// The table of configurable key names and their respective codes.
//
// We identify the virtual codes with a binary flag.
//
// When sending output using hardware scan codes is recommended as these do not
// get intercepted by DirectX and thus will work in more applications.
//
// When reading input the virtual codes will work accross more locales and envs.
//
// For backwards compatibility modifiers refer to the left key by default.
KEY_DEF key_table[] = {
    {"CTRL", SK_LEFT_CTRL, VK_LEFT_CTRL},
    {"LEFT_CTRL", SK_LEFT_CTRL, VK_LEFT_CTRL},
    {"RIGHT_CTRL", SK_RIGHT_CTRL, VK_RIGHT_CTRL},

    {"SHIFT", SK_LEFT_SHIFT, VK_LEFT_SHIFT},
    {"LEFT_SHIFT", SK_LEFT_SHIFT, VK_LEFT_SHIFT},
    {"RIGHT_SHIFT", SK_RIGHT_SHIFT, VK_RIGHT_SHIFT},

    {"ALT", SK_LEFT_ALT, VK_LEFT_ALT},
    {"LEFT_ALT", SK_LEFT_ALT, VK_LEFT_ALT},
    {"RIGHT_ALT", SK_RIGHT_ALT, VK_RIGHT_ALT},

    {"LEFT_WIN", SK_LEFT_WIN, VK_LEFT_WIN},
    {"RIGHT_WIN", SK_RIGHT_WIN, VK_RIGHT_WIN},

    {"BACKSPACE", SK_BACKSPACE, VK_BACKSPACE},
    {"CAPSLOCK", SK_CAPSLOCK, VK_CAPSLOCK},
    {"ENTER", SK_ENTER, VK_ENTER},
    {"ESCAPE", SK_ESCAPE, VK_ESCAPE},
    {"SPACE", SK_SPACE, VK_SPACE},
    {"TAB", SK_TAB, VK_TAB},

    {"NOOP", 0, 0},

    {"UP", SK_UP, VK_UP},
    {"LEFT", SK_LEFT, VK_LEFT},
    {"RIGHT", SK_RIGHT, VK_RIGHT},
    {"DOWN", SK_DOWN, VK_DOWN},

    {"LBRACKET", SK_LBRACKET, VK_LBRACKET},
    {"RBRACKET", SK_RBRACKET, VK_RBRACKET},
    {"BACKSLASH", SK_BACKSLASH, VK_BACKSLASH},
    {"APOSTROPHE", SK_APOSTROPHE, VK_APOSTROPHE},

    {"F1", SK_F1, VK_F1},
    {"F2", SK_F2, VK_F2},
    {"F3", SK_F3, VK_F3},
    {"F4", SK_F4, VK_F4},
    {"F5", SK_F5, VK_F5},
    {"F6", SK_F6, VK_F6},
    {"F7", SK_F7, VK_F7},
    {"F8", SK_F8, VK_F8},
    {"F9", SK_F9, VK_F9},
    {"F10", SK_F10, VK_F10},
    {"F11", SK_F11, VK_F11},
    {"F12", SK_F12, VK_F12},
    {"F13", SK_F13, VK_F13},
    {"F14", SK_F14, VK_F14},
    {"F15", SK_F15, VK_F15},
    {"F16", SK_F16, VK_F16},
    {"F17", SK_F17, VK_F17},
    {"F18", SK_F18, VK_F18},
    {"F19", SK_F19, VK_F19},
    {"F20", SK_F20, VK_F20},
    {"F21", SK_F21, VK_F21},
    {"F22", SK_F22, VK_F22},
    {"F23", SK_F23, VK_F23},

    {"KEY_0", SK_KEY_0, VK_KEY_0},
    {"KEY_1", SK_KEY_1, VK_KEY_1},
    {"KEY_2", SK_KEY_2, VK_KEY_2},
    {"KEY_3", SK_KEY_3, VK_KEY_3},
    {"KEY_4", SK_KEY_4, VK_KEY_4},
    {"KEY_5", SK_KEY_5, VK_KEY_5},
    {"KEY_6", SK_KEY_6, VK_KEY_6},
    {"KEY_7", SK_KEY_7, VK_KEY_7},
    {"KEY_8", SK_KEY_8, VK_KEY_8},
    {"KEY_9", SK_KEY_9, VK_KEY_9},

    {"KEY_A", SK_KEY_A, VK_KEY_A},
    {"KEY_B", SK_KEY_B, VK_KEY_B},
    {"KEY_C", SK_KEY_C, VK_KEY_C},
    {"KEY_D", SK_KEY_D, VK_KEY_D},
    {"KEY_E", SK_KEY_E, VK_KEY_E},
    {"KEY_F", SK_KEY_F, VK_KEY_F},
    {"KEY_G", SK_KEY_G, VK_KEY_G},
    {"KEY_H", SK_KEY_H, VK_KEY_H},
    {"KEY_I", SK_KEY_I, VK_KEY_I},
    {"KEY_J", SK_KEY_J, VK_KEY_J},
    {"KEY_K", SK_KEY_K, VK_KEY_K},
    {"KEY_L", SK_KEY_L, VK_KEY_L},
    {"KEY_M", SK_KEY_M, VK_KEY_M},
    {"KEY_N", SK_KEY_N, VK_KEY_N},
    {"KEY_O", SK_KEY_O, VK_KEY_O},
    {"KEY_P", SK_KEY_P, VK_KEY_P},
    {"KEY_Q", SK_KEY_Q, VK_KEY_Q},
    {"KEY_R", SK_KEY_R, VK_KEY_R},
    {"KEY_S", SK_KEY_S, VK_KEY_S},
    {"KEY_T", SK_KEY_T, VK_KEY_T},
    {"KEY_U", SK_KEY_U, VK_KEY_U},
    {"KEY_V", SK_KEY_V, VK_KEY_V},
    {"KEY_W", SK_KEY_W, VK_KEY_W},
    {"KEY_X", SK_KEY_X, VK_KEY_X},
    {"KEY_Y", SK_KEY_Y, VK_KEY_Y},
    {"KEY_Z", SK_KEY_Z, VK_KEY_Z},

    {"NUM0", SK_NUM0, VK_NUM0},
    {"NUM1", SK_NUM1, VK_NUM1},
    {"NUM2", SK_NUM2, VK_NUM2},
    {"NUM3", SK_NUM3, VK_NUM3},
    {"NUM4", SK_NUM4, VK_NUM4},
    {"NUM5", SK_NUM5, VK_NUM5},
    {"NUM6", SK_NUM6, VK_NUM6},
    {"NUM7", SK_NUM7, VK_NUM7},
    {"NUM8", SK_NUM8, VK_NUM8},
    {"NUM9", SK_NUM9, VK_NUM9},
    {"NUM_MULTIPLY", SK_NUM_MULTIPLY, VK_NUM_MULTIPLY},
    {"NUM_ADD", SK_NUM_ADD, VK_NUM_ADD},
    {"NUM_SUBTRACT", SK_NUM_SUBTRACT, VK_NUM_SUBTRACT},
    {"NUM_DECIMAL", SK_NUM_DECIMAL, VK_NUM_DECIMAL},
    {"NUM_DIVIDE", SK_NUM_DIVIDE, VK_NUM_DIVIDE},

    {"APPS", SK_APPS, VK_APPS},
    {"INSERT", SK_INSERT, VK_INSERT},
    {"DELETE", SK_DELETE, VK_DELETE},
    {"HOME", SK_HOME, VK_HOME},
    {"END", SK_END, VK_END},
    {"PAGE_UP", SK_PAGE_UP, VK_PAGE_UP},
    {"PAGE_DOWN", SK_PAGE_DOWN, VK_PAGE_DOWN},

    {"PRINT_SCREEN", SK_PRINT_SCREEN, VK_PRINT_SCREEN},
    {"NUMLOCK", SK_NUMLOCK, VK_NUMLOCK},
    {"SCROLLLOCK", SK_SCROLLLOCK, VK_SCROLLLOCK},
    {"PAUSE", SK_PAUSE, VK_PAUSE},

    {"MEDIA_PLAY_PAUSE", SK_MEDIA_PLAY_PAUSE, VK_MEDIA_PLAY_PAUSE},
    {"MEDIA_NEXT_TRACK", SK_MEDIA_NEXT_TRACK, VK_MEDIA_NEXT_TRACK},
    {"MEDIA_PREV_TRACK", SK_MEDIA_PREV_TRACK, VK_MEDIA_PREV_TRACK},
    {"MEDIA_STOP", SK_MEDIA_STOP, VK_MEDIA_STOP},
    {"VOLUME_MUTE", SK_VOLUME_MUTE, VK_VOLUME_MUTE},
    {"VOLUME_DOWN", SK_VOLUME_DOWN, VK_VOLUME_DOWN},
    {"VOLUME_UP", SK_VOLUME_UP, VK_VOLUME_UP},

    {"PLUS", SK_PLUS, VK_PLUS},
    {"COMMA", SK_COMMA, VK_COMMA},
    {"MINUS", SK_MINUS, VK_MINUS},
    {"PERIOD", SK_PERIOD, VK_PERIOD},

    {"US_SEMI", SK_US_SEMI, VK_US_SEMI}, // ;: key on US keyboards
    {"US_SLASH", SK_US_SLASH, VK_US_SLASH}, // /? key on US keyboards
    {"US_TILDE", SK_US_TILDE, VK_US_TILDE}, // `~ key on US keyboards
};

#define KEY_TABLE_LEN (sizeof(key_table) / sizeof(struct KeyDef))

// Shortcuts to common keys (useful for debugging/testing)
KEY_DEF * CTRL  = &key_table[0];
KEY_DEF * LCTRL = &key_table[1];
KEY_DEF * RCTRL = &key_table[2];
KEY_DEF * SHIFT = &key_table[3];
KEY_DEF * LSHIFT = &key_table[4];
KEY_DEF * RSHIFT = &key_table[5];
KEY_DEF * ALT   = &key_table[6];
KEY_DEF * CAPS  = &key_table[12];
KEY_DEF * ENTER = &key_table[13];
KEY_DEF * ESC   = &key_table[14];
KEY_DEF * SPACE = &key_table[15];
KEY_DEF * TAB   = &key_table[16];
KEY_DEF * NOOP  = &key_table[17];
KEY_DEF * MOUSE = &(struct KeyDef){"<MOUSE>", 0, MOUSE_DUMMY_VK};

KEY_DEF * find_key_def_by_name(char * name)
{
    if (name) {
        for (int i = 0; i < KEY_TABLE_LEN; ++i) {
            KEY_DEF * key = key_table + i;
            if (strcmp(key->name, name) == 0) {
                return key;
            }
        }
    }
    return 0;
}

KEY_DEF * find_key_def_by_scan_code(int code)
{
    for (int i = 0; i < KEY_TABLE_LEN; ++i) {
        KEY_DEF * key = key_table + i;
        if (key->scan_code == code) {
            return key;
        }
    }
    return 0;
}

KEY_DEF * find_key_def_by_virt_code(int code)
{
    for (int i = 0; i < KEY_TABLE_LEN; ++i) {
        KEY_DEF * key = key_table + i;
        if (key->virt_code == code) {
            return key;
        }
    }
    return 0;
}

// Defaults to the remappable key names per our definitions, but also
// handles more obscure codes that aren't available for remapping (yet).
// These fallback names are wrapped in angle brackets for log clarity.
// See: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
char * friendly_virt_code_name(int code)
{
    KEY_DEF * key = find_key_def_by_virt_code(code);
    if (key) return key->name;

    switch (code)
    {
    case MOUSE_DUMMY_VK:
        return "<MOUSE INPUT>";
    case 0x01:
        return "<MOUSE_LEFT>";
    case 0x02:
        return "<MOUSE_RIGHT>";
    case 0x03:
        return "<CANCEL>";
    case 0x04:
        return "<MOUSE_MIDDLE>";
    case 0x05:
        return "<MOUSE_X1>";
    case 0x06:
        return "<MOUSE_X2>";
    case 0x0C:
        return "<CLEAR>";
    case 0x10:
        return "<SHIFT_NO_DIR>";
    case 0x11:
        return "<CTRL_NO_DIR>";
    case 0x12:
        return "<ALT_NO_DIR>";
    case 0x15:
        return "<IME_KANA_OR_HANGUL>";
    case 0x16:
        return "<IME_ON>";
    case 0x17:
        return "<IME_JUNJA>";
    case 0x18:
        return "<IME_FINAL>";
    case 0x19:
        return "<IME_HANJA_OR_KANJI>";
    case 0x1A:
        return "<IME_OFF>";
    case 0x1C:
        return "<IME_CONVERT>";
    case 0x1D:
        return "<IME_NONCONVERT>";
    case 0x1E:
        return "<IME_ACCEPT>";
    case 0x1F:
        return "<IME_MODE_CHANGE>";
    case 0x29:
        return "<SELECT>";
    case 0x2A:
        return "<PRINT>";
    case 0x2B:
        return "<EXECUTE>";
    case 0x2F:
        return "<HELP>";
    case 0x5F:
        return "<SLEEP>";
    case 0x6C:
        return "<SEPARATOR>";
    case 0x87:
        return "<F24>";
    case 0xA6:
        return "<BROWSER_BACK>";
    case 0xA7:
        return "<BROWSER_FORWARD>";
    case 0xA8:
        return "<BROWSER_REFRESH>";
    case 0xA9:
        return "<BROWSER_STOP>";
    case 0xAA:
        return "<BROWSER_SEARCH>";
    case 0xAB:
        return "<BROWSER_FAVORITES>";
    case 0xAC:
        return "<BROWSER_HOME>";
    case 0xB4:
        return "<LAUNCH_MAIL>";
    case 0xB5:
        return "<LAUNCH_MEDIA_SELECT>";
    case 0xB6:
        return "<LAUNCH_APP1>";
    case 0xB7:
        return "<LAUNCH_APP2>";
    case 0xDF:
        return "<OEM_8>";
    case 0xE2:
        return "<OEM_102>";
    case 0xE5:
        return "<IME_PROCESS>";
    case 0xE6:
        return "<OEM_SPECIFIC>";
    case 0xE7:
        return "<PACKET>";
    case 0xF6:
        return "<ATTN>";
    case 0xF7:
        return "<CRSEL>";
    case 0xF8:
        return "<EXSEL>";
    case 0xF9:
        return "<EREOF>";
    case 0xFA:
        return "<PLAY>";
    case 0xFB:
        return "<ZOOM>";
    case 0xFC:
        return "<NONAME>";
    case 0xFD:
        return "<PA1>";
    case 0xFE:
        return "<OEM_CLEAR>";
    default:
        return "<UNKNOWN>";
    }
}

#endif