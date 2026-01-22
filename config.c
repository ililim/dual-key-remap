// IMPORTANT: must run on both windows and mac os
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_KEYS 16

enum State {
    IDLE,
    HELD_DOWN_ALONE,
    HELD_DOWN_WITH_OTHER,
};

struct Remap {
    struct KeyDef * from;
    struct KeyDef * to_when_alone[MAX_KEYS];
    int to_when_alone_count;
    struct KeyDef * to_with_other[MAX_KEYS];
    int to_with_other_count;
    enum State state;
    struct Remap * next;
};

// App struct for new config parsing
typedef struct {
    int debug;          // 0 = off, 1 = on
    int show_tray;      // 0 = hide, 1 = show (default: 1)
    int show_tutorial;  // 0 = hide, 1 = show (default: 0)
    struct Remap *remap_list;
} App;

/*
 * Helper to create an App instance with the proper default values.
 * This replaces the (invalid in C11) field initialisers that were
 * previously present inside the struct definition.
 */
static inline App app_default(void)
{
    return (App){
        .debug = 0,
        .show_tray = 1,     /* enabled by default */
        .show_tutorial = 0, /* disabled by default */
        .remap_list = NULL
    };
}

// Error handling macros
#define TRY(expr, msg) \
    do { if (!(expr)) { fprintf(stderr, "config error: %s\n", msg); return -1; } } while(0)

#define TRY_LINE(expr, msg, linenum) \
    do { if (!(expr)) { fprintf(stderr, "config error (line %d): %s\n", linenum, msg); return -1; } } while(0)

typedef struct {
    char key[256];
    char value[256];
} ConfigPair;

// forward declarations
extern int g_debug;
void trim_newline(char * str);

// Inline helper functions
static inline char* trim(char* s) {
    while (isspace((unsigned char)*s)) s++;
    if (!*s) return s;
    char* e = s + strlen(s) - 1;
    while (e >= s && isspace((unsigned char)*e)) *e-- = '\0';
    return s;
}

static inline void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

/**
 * Identify config version (we can tell by whether it has a dot)
 * 1 = legacy, 2 = new
 */
int identify_config_version(char * config_path)
{
    FILE * file = fopen(config_path, "r");
    if (!file) return -1;

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, ".") != NULL) {
            fclose(file);
            return 2;
        }
    }
    fclose(file);
    return 1;
}

// parse key=value, handling whitespace nicely
int parse_config_pair(char* line, ConfigPair* pair) {
    char buf[512];
    TRY(strlen(line) < sizeof(buf) - 1, "line too long");
    
    strcpy(buf, line);
    trim_newline(buf);
    
    char* trimmed = trim(buf);
    
    // skip blanks and comments
    if (trimmed[0] == '\0' || trimmed[0] == '#') return 0;
    
    char* eq = strchr(trimmed, '=');
    TRY(eq, "expected key=value format");
    
    *eq = '\0';
    char* key = trim(trimmed);
    char* value = trim(eq + 1);
    
    TRY(strlen(key) < sizeof(pair->key), "key too long");
    TRY(strlen(value) < sizeof(pair->value), "value too long");
    
    strcpy(pair->key, key);
    strcpy(pair->value, value);
    return 1;
}

/* Parse a boolean value expressed as "true"/"false"/"1"/"0" (case-sensitive).
 * Returns 1 for true, 0 for false, -1 for invalid.
 */
int parse_bool_value(char* value) {
    if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) return 1;
    if (strcmp(value, "false") == 0 || strcmp(value, "0") == 0) return 0;
    return -1; // invalid
}

// forward declarations for key lookup
struct KeyDef;
struct KeyDef * find_key_def_by_name(char * name);

void app_register_remap(App* app, struct Remap * remap)
{
    if (app->remap_list) {
        struct Remap * tail = app->remap_list;
        while (tail->next) tail = tail->next;
        tail->next = remap;
    } else {
        app->remap_list = remap;
    }
}

// parse remap.keyname.action format
int parse_remap_key(char* key, char* keyname_out, char* action_out) {
    if (strncmp(key, "remap.", 6) != 0) return 0; // not a remap key
    
    char* rest = key + 6; // skip "remap."
    char* dot = strchr(rest, '.');
    TRY(dot, "malformed remap key - missing second dot");
    
    int keyname_len = dot - rest;
    TRY(keyname_len < 256, "keyname too long");
    
    strncpy(keyname_out, rest, keyname_len);
    keyname_out[keyname_len] = '\0';
    to_uppercase(keyname_out);
    
    strcpy(action_out, dot + 1);
    return 1;
}

// find existing remap in the list or create new one with NOOP defaults
struct Remap* find_or_create_remap(App* app, struct KeyDef* key_def) {
    // look for existing remap first
    struct Remap* remap = app->remap_list;
    while (remap) {
        if (remap->from == key_def) {
            return remap;
        }
        remap = remap->next;
    }
    
    // create new one with NOOP defaults and add to list immediately
    extern struct KeyDef * NOOP;  // assume NOOP is defined in keys.c
    remap = malloc(sizeof(struct Remap));
    memset(remap, 0, sizeof(struct Remap));
    remap->from = key_def;
    remap->to_when_alone[0] = NOOP;
    remap->to_when_alone_count = 1;
    remap->to_with_other[0] = NOOP;
    remap->to_with_other_count = 1;
    app_register_remap(app, remap);
    return remap;
}

int parse_key_list_new(char* value, struct KeyDef** keys_out, int* count_out, int linenum) {
    *count_out = 0;
    char buf[512];
    TRY_LINE(strlen(value) < sizeof(buf), "value too long", linenum);
    strcpy(buf, value);
    
    char* token = strtok(buf, ",");
    while (token && *count_out < MAX_KEYS) {
        token = trim(token);
        to_uppercase(token);
        
        struct KeyDef* key_def = find_key_def_by_name(token);
        TRY_LINE(key_def, "invalid key name", linenum);
        
        keys_out[(*count_out)++] = key_def;
        token = strtok(NULL, ",");
    }
    
    TRY_LINE(*count_out > 0, "no keys specified", linenum);
    return 0;
}

int parse_new_config_line(App* app, char* line, int linenum) {
    ConfigPair pair;
    int result = parse_config_pair(line, &pair);
    
    if (result <= 0) return result; // skip or error
    
    /* handle simple on/off settings */
    if (strcmp(pair.key, "debug") == 0 ||
        strcmp(pair.key, "show_tray") == 0 ||
        strcmp(pair.key, "show_tutorial") == 0) {

        int bool_val = parse_bool_value(pair.value);
        TRY_LINE(bool_val != -1, "value must be true/false/1/0", linenum);

        if (strcmp(pair.key, "debug") == 0) {
            app->debug = bool_val;
        } else if (strcmp(pair.key, "show_tray") == 0) {
            app->show_tray = bool_val;
        } else if (strcmp(pair.key, "show_tutorial") == 0) {
            app->show_tutorial = bool_val;
        }
        return 0;
    }
    
    // handle remap.capslock.when_alone = escape
    // handle remap.capslock.with_other = ctrl
    char keyname[256], action[256];
    int remap_result = parse_remap_key(pair.key, keyname, action);
    if (remap_result == 1) {
        struct KeyDef* from_key = find_key_def_by_name(keyname);
        TRY_LINE(from_key, "invalid key name", linenum);
        
        struct Remap* remap = find_or_create_remap(app, from_key);
        
        if (strcmp(action, "when_alone") == 0) {
            if (parse_key_list_new(pair.value, remap->to_when_alone, &remap->to_when_alone_count, linenum) != 0) {
                return -1;
            }
        } else if (strcmp(action, "with_other") == 0) {
            if (parse_key_list_new(pair.value, remap->to_with_other, &remap->to_with_other_count, linenum) != 0) {
                return -1;
            }
        } else {
            TRY_LINE(0, "invalid action - must be when_alone or with_other", linenum);
        }
        
        return 0;
    } else if (remap_result == -1) {
        return -1; // malformed remap key (error already printed by TRY)
    }
    
    TRY_LINE(0, "unknown setting", linenum);
    return -1; // unreachable but keeps compiler happy
}
