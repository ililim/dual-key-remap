#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "input.h"
#include "keys.c"

#ifdef _WIN32
#   define strcasecmp _stricmp
#endif

// Types
// --------------------------------------

enum State {
    IDLE,
    HELD_DOWN_ALONE,
    HELD_DOWN_WITH_OTHER,
};

#define MAX_KEYS 16

struct Remap
{
    KEY_DEF * from;
    KEY_DEF * to_when_alone[MAX_KEYS];
    int to_when_alone_count;
    KEY_DEF * to_with_other[MAX_KEYS];
    int to_with_other_count;

    enum State state;

    struct Remap * next;
};

// Globals
// --------------------------------------

int g_paused = 0;
int g_debug = 0;
int g_show_tray = 1;
char g_last_error[256] = {0};
struct Remap * g_remap_list;
struct Remap * g_remap_parsee = 0;

// Debug Logging
// --------------------------------------

extern int can_print(void);

#define log_error(fmt, ...) do { \
    if (g_debug && can_print()) { \
        printf(fmt, ##__VA_ARGS__); \
    } \
    sprintf(g_last_error, fmt, ##__VA_ARGS__); \
} while(0)

#define log_info(fmt, ...) do { \
    if (g_debug && can_print()) { \
        printf(fmt, ##__VA_ARGS__); \
    } \
} while(0)

char * fmt_dir(enum Direction dir)
{
    return dir ? "DOWN" : "UP";
}

int log_indent_level = 0;
int log_counter = 1;
void print_log_prefix()
{
    log_info("\n%03d. ", log_counter++);
    for (int i = 0; i < log_indent_level; i++)
    {
        log_info("\t");
    }
}

void log_handle_input_start(int scan_code, int virt_code, int dir, int is_injected)
{
    print_log_prefix();
    log_info("[%s] %s %s (scan:0x%02x virt:0x%02x)",
        is_injected ? "output" : "input",
        friendly_virt_code_name(virt_code),
        fmt_dir(dir),
        scan_code,
        virt_code);
    log_indent_level++;
}

void log_handle_input_end(int scan_code, int virt_code, int dir, int is_injected, int block_input)
{
    log_indent_level--;
    if (block_input) {
        print_log_prefix();
        log_info("#blocked-input# %s %s",
            friendly_virt_code_name(virt_code),
            fmt_dir(dir));
    }
}

void log_send_input(char * remap_name, KEY_DEF * key, int dir)
{
    print_log_prefix();
    log_info("(sending:%s) %s %s",
        remap_name,
        key ? key->name : "???",
        fmt_dir(dir));
}

// Remapping
// -------------------------------------

void register_remap(struct Remap * remap)
{
    if (g_remap_list) {
        struct Remap * tail = g_remap_list;
        while (tail->next) tail = tail->next;
        tail->next = remap;
    } else {
        g_remap_list = remap;
    }
}

struct Remap * find_remap_for_virt_code(int virt_code)
{
    struct Remap * remap = g_remap_list;
    while(remap) {
        if (remap->from->virt_code == virt_code) {
            return remap;
        }
        remap = remap->next;
    }
    return 0;
}

void send_key_def_input(char * input_name, KEY_DEF * key_def, enum Direction dir)
{
    log_send_input(input_name, key_def, dir);
    send_input(key_def->scan_code, key_def->virt_code, dir);
}

void send_multi_key_def_input(char * input_name, KEY_DEF ** key_defs, int count, enum Direction dir)
{
    for (int i = 0; i < count; i++) {
        log_send_input(input_name, key_defs[i], dir);
        send_input(key_defs[i]->scan_code, key_defs[i]->virt_code, dir);
    }
}

/* @return block_input */
int event_remapped_key_down(struct Remap * remap)
{
    if (remap->state == IDLE) {
        remap->state = HELD_DOWN_ALONE;
    }
    return 1;
}

/* @return block_input */
int event_remapped_key_up(struct Remap * remap)
{
    if (remap->state == HELD_DOWN_WITH_OTHER) {
        remap->state = IDLE;
        send_multi_key_def_input("with_other", remap->to_with_other, remap->to_with_other_count, UP);
    } else {
        remap->state = IDLE;
        for (int i = 0; i < remap->to_when_alone_count; i++) {
            send_key_def_input("when_alone", remap->to_when_alone[i], DOWN);
            send_key_def_input("when_alone", remap->to_when_alone[i], UP);
        }
    }
    return 1;
}

/* @return block_input */
int event_other_input()
{
    struct Remap * remap = g_remap_list;
    while(remap) {
        if (remap->state == HELD_DOWN_ALONE) {
            remap->state = HELD_DOWN_WITH_OTHER;
            send_multi_key_def_input("with_other", remap->to_with_other, remap->to_with_other_count, DOWN);
        }
        remap = remap->next;
    }
    return 0;
}


/* @return block_input */
int handle_input(int scan_code, int virt_code, int direction, int is_injected)
{
    log_handle_input_start(scan_code, virt_code, direction, is_injected);

    if (g_paused) {
        log_handle_input_end(scan_code, virt_code, direction, is_injected, 0);
        return 0;
    }

    // Note: injected keys are not remapped to avoid recursion loops and to support key swapping
    struct Remap * remap_for_input = is_injected ? 0 : find_remap_for_virt_code(virt_code);
    int block_input = 0;

    if (!remap_for_input) {
        block_input = event_other_input();
    } else {
        block_input = direction == DOWN
            ? event_remapped_key_down(remap_for_input)
            : event_remapped_key_up(remap_for_input);
    }
    log_handle_input_end(scan_code, virt_code, direction, is_injected, block_input);
    return block_input;
}

// Config
// ---------------------------------

void trim_newline(char * str)
{
    str[strcspn(str, "\r\n")] = 0;
}

int parsee_is_valid()
{
    return g_remap_parsee &&
        g_remap_parsee->from &&
        g_remap_parsee->to_when_alone_count > 0 &&
        g_remap_parsee->to_with_other_count > 0;
}

int parse_key_list(char * value, KEY_DEF ** keys_out, int * count_out, int linenum)
{
    *count_out = 0;
    char buf[512];
    if (strlen(value) >= sizeof(buf)) {
        log_error("Config error (line %d): value too long\n", linenum);
        return 1;
    }
    strcpy(buf, value);
    
    char * token = strtok(buf, ",");
    while (token && *count_out < MAX_KEYS) {
        while (isspace((unsigned char)*token)) token++;
        char * end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) *end-- = '\0';
        
        KEY_DEF * key_def = find_key_def_by_name(token);
        if (!key_def) {
            log_error(
                    "Config error (line %d): invalid key name '%s'\n"
                    "See the online docs for the latest list of key names.\n",
                    linenum, token);
            return 1;
        }
        keys_out[(*count_out)++] = key_def;
        token = strtok(NULL, ",");
    }
    
    if (*count_out == 0) {
        log_error("Config error (line %d): no keys specified\n", linenum);
        return 1;
    }
    
    return 0;
}

/* @return error */
int load_config_line(char *line, int linenum)
{
    char buf[256];
    if (strlen(line) >= sizeof(buf) - 1) {
        log_error(
                "Config error (line %d): line too long (max %zu chars)\n",
                linenum, sizeof(buf) - 2);
        return 1;
    }
    strcpy(buf, line);
    line = buf;

    // ignore blanks / comments
    trim_newline(line);
    if (line[0] == '\0' || line[0] == '#')
        return 0;

    // split key = value
    char *after_eq = strchr(line, '=');
    if (!after_eq) {
        log_error(
                "Config error (line %d): expected key=value\n", linenum);
        return 1;
    }
    *after_eq++ = '\0'; // terminate key, advance to value

    // right-trim key, left-trim value
    while (*line && isspace((unsigned char)line[strlen(line) - 1]))
        line[strlen(line) - 1] = '\0';
    while (isspace((unsigned char)*after_eq))
        ++after_eq;

    // toggle debug mode
    if (strcmp(line, "debug") == 0) {
        if (strcmp(after_eq, "1") == 0 || strcasecmp(after_eq, "true")  == 0) {
            g_debug = 1;
            return 0;
        }
        if (strcmp(after_eq, "0") == 0 || strcasecmp(after_eq, "false") == 0) {
            g_debug = 0;
            return 0;
        }
        log_error(
                "Config error (line %d): debug must be 0/1/true/false\n",
                linenum);
        return 1;
    }

    // toggle tray visibility
    if (strcmp(line, "show_tray") == 0) {
        if (strcmp(after_eq, "1") == 0 || strcasecmp(after_eq, "true")  == 0) {
            g_show_tray = 1;
            return 0;
        }
        if (strcmp(after_eq, "0") == 0 || strcasecmp(after_eq, "false") == 0) {
            g_show_tray = 0;
            return 0;
        }
        log_error(
                "Config error (line %d): show_tray must be 0/1/true/false\n",
                linenum);
        return 1;
    }

    // remap directives
    int field = 0;                  // 1 = remap_key, 2 = when_alone, 3 = with_other
    if      (strcmp(line, "remap_key")  == 0) field = 1;
    else if (strcmp(line, "when_alone") == 0) field = 2;
    else if (strcmp(line, "with_other") == 0) field = 3;

    if (field) {
        // allocate the working remap if this is the first line of a block
        if (!g_remap_parsee) {
            g_remap_parsee = malloc(sizeof(struct Remap));
            memset(g_remap_parsee, 0, sizeof(struct Remap));
        }

        // fill the appropriate field & catch duplicate remap_key
        if (field == 1) {
            if (g_remap_parsee->from && !parsee_is_valid()) {
                log_error(
                        "Config error (line %d): Incomplete remapping.\n"
                        "Each remap needs remap_key, when_alone and with_other\n"
                        "before another remap_key.\n",
                        linenum);
                return 1;
            }
            KEY_DEF *key_def = find_key_def_by_name(after_eq);
            if (!key_def) {
                log_error(
                        "Config error (line %d): invalid key name '%s'\n"
                        "See the online docs for the latest list of key names.\n",
                        linenum, after_eq);
                return 1;
            }
            g_remap_parsee->from = key_def;
        } else if (field == 2) { // when_alone=
            if (parse_key_list(after_eq, g_remap_parsee->to_when_alone, &g_remap_parsee->to_when_alone_count, linenum)) {
                return 1;
            }
        } else { // with_other=
            if (parse_key_list(after_eq, g_remap_parsee->to_with_other, &g_remap_parsee->to_with_other_count, linenum)) {
                return 1;
            }
        }

        // commit when block is complete
        if (parsee_is_valid()) {
            register_remap(g_remap_parsee);
            g_remap_parsee = 0;
        }
        return 0;
    }

    // anything else is unknown
    log_error("Config error (line %d): invalid setting '%s'\n",
            linenum, line);
    return 1;
}

void reset_config()
{
    free(g_remap_parsee);
    g_remap_parsee = 0;
    while (g_remap_list) {
        struct Remap * remap = g_remap_list;
        g_remap_list = remap->next;
        free(remap);
    }
    g_remap_list = 0;
}
