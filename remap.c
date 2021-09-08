#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "input.h"
#include "keys.c"

// Types
// --------------------------------------

enum State {
    IDLE,
    HELD_DOWN_ALONE,
    HELD_DOWN_WITH_OTHER,
};

struct Remap
{
    KEY_DEF * from;
    KEY_DEF * to_when_alone;
    KEY_DEF * to_with_other;

    enum State state;

    struct Remap * next;
};

// Globals
// --------------------------------------

int g_debug = 0;
struct Remap * g_remap_list;
struct Remap * g_remap_parsee = NULL;

// Debug Logging
// --------------------------------------

char * fmt_dir(enum Direction dir) {
    return dir ? "DOWN" : "UP  ";
}

int prev_dbg_scan_code = 0;
int prev_dbg_virt_code = 0;
int prev_dbg_direction = 0;
void log_input(int scan_code, int virt_code, int dir)
{
    if (!g_debug) return;

    // To avoid duplicate logs, only log if key input changed
    if (prev_dbg_scan_code == prev_dbg_scan_code
        && prev_dbg_virt_code == virt_code
        && prev_dbg_direction == dir) return;
    prev_dbg_scan_code = scan_code;
    prev_dbg_virt_code = virt_code;
    prev_dbg_direction = dir;

    KEY_DEF * key = find_key_def_by_virt_code(virt_code);
    printf("(input) %s %s [scan:%d virt:%d]\n",
        key ? key->name : "KEY",
        fmt_dir(dir),
        scan_code,
        virt_code);
}

char * prev_event;
void log_event(char * event)
{
    if (!g_debug) return;
    if (prev_event == event) return;
    prev_event = event;
    printf("(event) %s\n", event);
}

void log_send(KEY_DEF * key, int dir)
{
    if (!g_debug) return;
    printf("(send) %s %s\n",
        key ? key->name : "???",
        fmt_dir(dir));
}

// Remapping
// -------------------------------------

struct Remap * new_remap(KEY_DEF * from, KEY_DEF * to_when_alone, KEY_DEF * to_with_other)
{
    struct Remap * remap = malloc(sizeof(struct Remap));
    remap->from = from;
    remap->to_when_alone = to_when_alone;
    remap->to_with_other = to_with_other;
    remap->state = IDLE;
    remap->next = NULL;
    return remap;
}

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
    return NULL;
}

void send_key_def_input(KEY_DEF * key_def, enum Direction dir)
{
    log_send(key_def, dir);
    send_input(key_def->scan_code, key_def->virt_code, dir);
}

/* @return swallow_input */
int event_remapped_key_down(struct Remap * remap)
{
    log_event("event_remapped_key_down");
    if (remap->state == IDLE) {
        remap->state = HELD_DOWN_ALONE;
    }
    return 1;
}

/* @return swallow_input */
int event_remapped_key_up(struct Remap * remap)
{
    log_event("event_remapped_key_up");
    if (remap->state == HELD_DOWN_WITH_OTHER) {
        remap->state = IDLE;
        send_key_def_input(remap->to_with_other, UP);
    } else {
        remap->state = IDLE;
        send_key_def_input(remap->to_when_alone, DOWN);
        send_key_def_input(remap->to_when_alone, UP);
    }
    return 1;
}

int event_other_input()
{
    struct Remap * remap = g_remap_list;
    while(remap) {
        if (remap->state == HELD_DOWN_ALONE) {
            remap->state = HELD_DOWN_WITH_OTHER;
            send_key_def_input(remap->to_with_other, DOWN);
        }
        remap = remap->next;
    }
    return 0;
}


/* @return swallow_input */
int handle_input(int scan_code, int virt_code, int direction, int is_injected)
{
    if (!is_injected) log_input(scan_code, virt_code, direction);
    struct Remap * remap_for_input = find_remap_for_virt_code(virt_code);

    if (remap_for_input && !is_injected) {
        if (direction == DOWN) {
            return event_remapped_key_down(remap_for_input);
        }
        else {
            return event_remapped_key_up(remap_for_input);
        }
    } else {
        return event_other_input();
    }
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
        g_remap_parsee->to_when_alone &&
        g_remap_parsee->to_with_other;
}

/* @return error */
int load_config_line(char * line, int linenum)
{
    trim_newline(line);

    // Ignore comments and empty lines
    if (line[0] == '#' || line[0] == '\0') {
        return 0;
    }

    // Handle config declaration
    if (strstr(line, "debug=1")) {
        g_debug = 1;
        return 0;
    }
    if (strstr(line, "debug=0")) {
        g_debug = 0;
        return 0;
    }

    // Handle key remappings
    char * after_eq = (char *)strchr(line, '=');
    if (!after_eq) {
        printf("Config error (line %d): Couldn't understand '%s'.\n", linenum, line);
        return 1;
    }
    char * key_name = after_eq + 1;
    KEY_DEF * key_def = find_key_def_by_name(key_name);
    if (!key_def) {
        printf("Config error (line %d): Invalid key name '%s'.\n", linenum, key_name);
        return 1;
    }

    if (g_remap_parsee == NULL) {
        g_remap_parsee = new_remap(NULL, NULL, NULL);
    }

    if (strstr(line, "remap_key=")) {
        if (g_remap_parsee->from && !parsee_is_valid()) {
            printf("Config error (line %d): Incomplete remapping.\n"
                   "Each remapping must have a 'remap_key', 'when_alone', and 'with_other'.\n",
                   linenum);
            return 1;
        }
        g_remap_parsee->from = key_def;
    } else if (strstr(line, "when_alone=")) {
        g_remap_parsee->to_when_alone = key_def;
    } else if (strstr(line, "with_other=")) {
        g_remap_parsee->to_with_other = key_def;
    } else {
        after_eq[0] = 0;
        printf("Config error (line %d): Invalid setting '%s'.\n", linenum, line);
        return 1;
    }

    if (parsee_is_valid()) {
        register_remap(g_remap_parsee);
        g_remap_parsee = NULL;
    }

    return 0;
}

void reset_config()
{
    free(g_remap_parsee);
    g_remap_parsee = NULL;
    while (g_remap_list) {
        struct Remap * remap = g_remap_list;
        g_remap_list = remap->next;
        free(remap);
    }
    g_remap_list = NULL;
}
