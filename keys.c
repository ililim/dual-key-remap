#include <stddef.h>

#ifndef KEYS_C
#define KEYS_C

struct KeyDef {
    char * name;
    int scan_code;
    int virt_code;
};
typedef const struct KeyDef KEY_DEF;

#define VK_LSHIFT 0xA0
#define VK_RSHIFT 0xA1
#define VK_LCONTROL 0xA2
#define VK_RCONTROL 0xA3
#define VK_LMENU 0xA4
#define VK_RMENU 0xA5
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_LWIN 0x5B
#define VK_RWIN 0x5C
#define VK_ESCAPE 0x1B
#define VK_BACK 0x08
#define VK_SPACE 0x20
#define VK_CAPITAL 0x14

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
    // Hardware scan code keys
    {"CTRL", 0x001D, VK_LCONTROL},
    {"LEFT_CTRL", 0x001D, VK_LCONTROL},
    {"RIGHT_CTRL", 0xE01D, VK_RCONTROL},
    {"SHIFT", 0x002A, VK_LSHIFT},
    {"LEFT_SHIFT", 0x002A, VK_LSHIFT},
    {"RIGHT_SHIFT", 0x0036, VK_RSHIFT},
    {"ALT", 0x0038, VK_LMENU},
    {"LEFT_ALT", 0x0038, VK_LMENU},
    {"RIGHT_ALT", 0xE038, VK_RMENU},
    {"LEFT_WINDOWS", 0xE05B, VK_LWIN},
    {"RIGHT_WINDOWS", 0xE05C, VK_RWIN},

    {"ESCAPE", 0x01, VK_ESCAPE},
    {"BACKSPACE", 0x0E, VK_BACK},
    {"TAB", 0x0F, VK_TAB},
    {"SPACE", 0x39, VK_SPACE},
    {"CAPSLOCK", 0x3A, VK_CAPITAL},
    {"ENTER", 0x1C, VK_RETURN},

    // {"Q", 0x0010, 10100},
    // {"W", 0x0011, 10100},
    // {"E", 0x0012, 10100},
    // {"R", 0x0013, 10100},
    // {"T", 0x0014, 10100},
    // {"Y", 0x0015, 10100},
    // {"U", 0x0016, 10100},
    // {"I", 0x0017, 10100},
    // {"O", 0x0018, 10100},
    // {"P", 0x0019, 10100},
    // {"D", 0x0020, 10100},
    // {"F", 0x0021, 10100},
    // {"G", 0x0022, 10100},
    // {"H", 0x0023, 100},
    // {"J", 0x0024, 100},
    // {"K", 0x0025, 100},
    // {"L", 0x0026, 100},
    // {"Z", 0x002C, 100},
    // {"X", 0x002D, 100},
    // {"C", 0x002E, 100},
    // {"V", 0x002F, 100},
    // {"B", 0x0030, 100},
    // {"N", 0x0031, 100},
    // {"M", 0x0032, 100},
    // {"A", 0x001E, 100},
    // {"S", 0x001F, 100},
};

#define KEY_TABLE_LEN (sizeof(key_table) / sizeof(struct KeyDef))

// Shortcuts to common keys (useful for debugging/testing)
KEY_DEF * CTRL  = &key_table[0];
KEY_DEF * SHIFT = &key_table[3];
KEY_DEF * ALT   = &key_table[6];
KEY_DEF * ESC   = &key_table[11];
KEY_DEF * TAB   = &key_table[13];
KEY_DEF * SPACE = &key_table[14];
KEY_DEF * CAPS  = &key_table[15];
KEY_DEF * ENTER = &key_table[16];

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
