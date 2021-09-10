#include <stddef.h>

#ifndef KEYS_C
#define KEYS_C

struct KeyDef {
    char * name;
    int scan_code;
    int virt_code;
};
typedef const struct KeyDef KEY_DEF;

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

#define VK_PLUS 0xBB
#define VK_COMMA 0xBC
#define VK_MINUS 0xBD
#define VK_PERIOD 0xBE

#define VK_US_SEMI 0xBA // ;: key on US keyboards
#define VK_US_SLASH 0xBF // /? key on US keyboards
#define VK_US_TILDE 0xC0 // `~ key on US keyboards

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

#define SK_INSERT 0x52
#define SK_DELETE 0x53
#define SK_HOME 0x47
#define SK_END 0x4f
#define SK_PAGE_UP 0x49
#define SK_PAGE_DOWN 0x51

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

    {"UP", SK_UP, VK_UP},
    {"LEFT", SK_LEFT, VK_LEFT},
    {"RIGHT", SK_RIGHT, VK_RIGHT},
    {"DOWN", SK_DOWN, VK_DOWN},

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

    {"INSERT", SK_INSERT, VK_INSERT},
    {"DELETE", SK_DELETE, VK_DELETE},
    {"HOME", SK_HOME, VK_HOME},
    {"END", SK_END, VK_END},
    {"PAGE_UP", SK_PAGE_UP, VK_PAGE_UP},
    {"PAGE_DOWN", SK_PAGE_DOWN, VK_PAGE_DOWN},

    {"PRINT_SCREEN", 0, VK_PRINT_SCREEN},
    {"NUMLOCK", 0, VK_NUMLOCK},
    {"SCROLLLOCK", 0, VK_SCROLLLOCK},
    {"PAUSE", 0, VK_PAUSE},

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
KEY_DEF * SHIFT = &key_table[3];
KEY_DEF * ALT   = &key_table[6];
KEY_DEF * CAPS  = &key_table[12];
KEY_DEF * ENTER = &key_table[13];
KEY_DEF * ESC   = &key_table[14];
KEY_DEF * SPACE = &key_table[15];
KEY_DEF * TAB   = &key_table[16];

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
    return NULL;
}

KEY_DEF * find_key_def_by_scan_code(int code)
{
    for (int i = 0; i < KEY_TABLE_LEN; ++i) {
        KEY_DEF * key = key_table + i;
        if (key->scan_code == code) {
            return key;
        }
    }
    return NULL;
}

KEY_DEF * find_key_def_by_virt_code(int code)
{
    for (int i = 0; i < KEY_TABLE_LEN; ++i) {
        KEY_DEF * key = key_table + i;
        if (key->virt_code == code) {
            return key;
        }
    }
    return NULL;
}

#endif
