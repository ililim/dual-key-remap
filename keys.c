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

// Generate enum from keys.def
#define KEY(n,s,v) k_##n,
enum {
#include "keys.def"
    k_COUNT
};
#undef KEY

// Generate key table from keys.def
#define KEY(n,s,v) { #n, s, v },
KEY_DEF key_table[] = {
#include "keys.def"
};
#undef KEY

#define KEY_TABLE_LEN (sizeof(key_table) / sizeof(struct KeyDef))


// Shortcuts to common keys and some special codes
#define MOUSE_DUMMY_VK 0xFF
#define SK_LEFT_SHIFT   0x002A
#define VK_LEFT_SHIFT   0xA0
#define VK_RIGHT_SHIFT  0xA1
KEY_DEF * CTRL  = &key_table[k_CTRL];
KEY_DEF * LCTRL = &key_table[k_LEFT_CTRL];
KEY_DEF * RCTRL = &key_table[k_RIGHT_CTRL];
KEY_DEF * SHIFT = &key_table[k_SHIFT];
KEY_DEF * LSHIFT = &key_table[k_LEFT_SHIFT];
KEY_DEF * RSHIFT = &key_table[k_RIGHT_SHIFT];
KEY_DEF * ALT   = &key_table[k_ALT];
KEY_DEF * CAPS  = &key_table[k_CAPSLOCK];
KEY_DEF * ENTER = &key_table[k_ENTER];
KEY_DEF * ESC   = &key_table[k_ESCAPE];
KEY_DEF * SPACE = &key_table[k_SPACE];
KEY_DEF * TAB   = &key_table[k_TAB];
KEY_DEF * NOOP  = &key_table[k_NOOP];
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