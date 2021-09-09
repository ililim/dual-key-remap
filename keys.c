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

#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A

#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_HOME 0x24
#define VK_END 0x23
#define VK_PAGE_UP 0x21
#define VK_PAGE_DOWN 0x22

#define VK_PLUS 0xBB
#define VK_COMMA 0xBC
#define VK_MINUS 0xBD
#define VK_PERIOD 0xBE

#define VK_OEM_1 0xBA // ;: key on US keyboards
#define VK_OEM_2 0xBF // /? key on US keyboards
#define VK_OEM_3 0xC0 // `~ key on US keyboards

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

#define SK_0 0x0B
#define SK_1 0x02
#define SK_2 0x03
#define SK_3 0x04
#define SK_4 0x05
#define SK_5 0x06
#define SK_6 0x07
#define SK_7 0x08
#define SK_8 0x09
#define SK_9 0x0A

#define SK_A 0x1E
#define SK_B 0x30
#define SK_C 0x2E
#define SK_D 0x20
#define SK_E 0x12
#define SK_F 0x21
#define SK_G 0x22
#define SK_H 0x23
#define SK_I 0x17
#define SK_J 0x24
#define SK_K 0x25
#define SK_L 0x26
#define SK_M 0x32
#define SK_N 0x31
#define SK_O 0x18
#define SK_P 0x19
#define SK_Q 0x10
#define SK_R 0x13
#define SK_S 0x1F
#define SK_T 0x14
#define SK_U 0x16
#define SK_V 0x2F
#define SK_W 0x11
#define SK_X 0x2D
#define SK_Y 0x15
#define SK_Z 0x2C

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

#define SK_OEM_1 0x27 // ;: key on US keyboards
#define SK_OEM_2 0x35 // /? key on US keyboards
#define SK_OEM_3 0x29 // `~ key on US keyboards

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

    {"0", SK_0, VK_0},
    {"1", SK_1, VK_1},
    {"2", SK_2, VK_2},
    {"3", SK_3, VK_3},
    {"4", SK_4, VK_4},
    {"5", SK_5, VK_5},
    {"6", SK_6, VK_6},
    {"7", SK_7, VK_7},
    {"8", SK_8, VK_8},
    {"9", SK_9, VK_9},

    {"A", SK_A, VK_A},
    {"B", SK_B, VK_B},
    {"C", SK_C, VK_C},
    {"D", SK_D, VK_D},
    {"E", SK_E, VK_E},
    {"F", SK_F, VK_F},
    {"G", SK_G, VK_G},
    {"H", SK_H, VK_H},
    {"I", SK_I, VK_I},
    {"J", SK_J, VK_J},
    {"K", SK_K, VK_K},
    {"L", SK_L, VK_L},
    {"M", SK_M, VK_M},
    {"N", SK_N, VK_N},
    {"O", SK_O, VK_O},
    {"P", SK_P, VK_P},
    {"Q", SK_Q, VK_Q},
    {"R", SK_R, VK_R},
    {"S", SK_S, VK_S},
    {"T", SK_T, VK_T},
    {"U", SK_U, VK_U},
    {"V", SK_V, VK_V},
    {"W", SK_W, VK_W},
    {"X", SK_X, VK_X},
    {"Y", SK_Y, VK_Y},
    {"Z", SK_Z, VK_Z},

    {"INSERT", SK_INSERT, VK_INSERT},
    {"DELETE", SK_DELETE, VK_DELETE},
    {"HOME", SK_HOME, VK_HOME},
    {"END", SK_END, VK_END},
    {"PAGE_UP", SK_PAGE_UP, VK_PAGE_UP},
    {"PAGE_DOWN", SK_PAGE_DOWN, VK_PAGE_DOWN},

    {"PLUS", SK_PLUS, VK_PLUS},
    {"COMMA", SK_COMMA, VK_COMMA},
    {"MINUS", SK_MINUS, VK_MINUS},
    {"PERIOD", SK_PERIOD, VK_PERIOD},

    {"OEM_1", SK_OEM_1, VK_OEM_1}, // ;: key on US keyboards
    {"OEM_2", SK_OEM_2, VK_OEM_2}, // /? key on US keyboards
    {"OEM_3", SK_OEM_3, VK_OEM_3}, // `~ key on US keyboards
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
