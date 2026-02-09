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

#define MAX_SEQ 8

enum State {
    IDLE,
    HELD_DOWN_ALONE,
    HELD_DOWN_WITH_OTHER,
};

struct Remap
{
    KEY_DEF * from;

    KEY_DEF * when_alone_keys[MAX_SEQ];
    int when_alone_count;

    KEY_DEF * with_other_keys[MAX_SEQ];
    int with_other_count;

    unsigned long long key_down_time;
    int timeout_ms;

    enum State state;

    struct Remap * next;
};

// Globals
// --------------------------------------

int g_paused = 0;
int g_debug = 0;
int g_show_tray = 1;
int g_timeout_ms = 0;
char g_last_error[256] = {0};
struct Remap * g_remap_list;
struct Remap * g_remap_parsee = 0;

// Time
// --------------------------------------

#ifdef _WIN32
static unsigned long long get_tick_count_ms(void) {
    return GetTickCount64();
}
#else
#include <time.h>
static unsigned long long get_tick_count_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long long)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}
#endif

unsigned long long (*get_time_ms)(void) = get_tick_count_ms; // overrideable for testing

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

// Remapping
// -------------------------------------

struct Remap * new_remap(KEY_DEF * from)
{
    struct Remap * remap = calloc(1, sizeof(struct Remap));
    remap->from = from;
    remap->state = IDLE;
    remap->when_alone_count = -1;
    remap->with_other_count = -1;
    remap->timeout_ms = -1;
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
    return 0;
}

static void send_sequence(const char *name, KEY_DEF **keys, int count, enum Direction dir)
{
    if (count <= 0) return;

    if (g_debug && can_print()) {
        print_log_prefix();
        log_info("(sending:%s) ", name);
        for (int i = 0; i < count; ++i) {
            log_info("%s", keys[i]->name);
            if (i+1 < count) log_info("+");
        }
        log_info(" %s", fmt_dir(dir));
    }

    if (dir == DOWN) {
        for (int i = 0; i < count; ++i)
            send_input(keys[i]->scan_code, keys[i]->virt_code, DOWN);
    } else {
        for (int i = count - 1; i >= 0; --i)
            send_input(keys[i]->scan_code, keys[i]->virt_code, UP);
    }
}

static int parse_key_sequence(char *value, KEY_DEF **out)
{
    while (*value && isspace((unsigned char)*value)) ++value;
    if (*value == '\0') return 0;
    if (strcasecmp(value, "NOOP") == 0) return 0;

    int count = 0;
    char *p = value;
    while (*p) {
        while (isspace((unsigned char)*p)) ++p;
        char *start = p;

        while (*p && *p != '+') ++p;

        char saved = *p;
        *p = '\0';

        size_t len = strlen(start);
        while (len > 0 && isspace((unsigned char)start[len-1])) start[--len] = '\0';

        if (len > 0) {
            for (char *c = start; *c; c++) *c = toupper((unsigned char)*c);
            KEY_DEF *k = find_key_def_by_name(start);
            if (!k) return -1;
            if (count >= MAX_SEQ) return -1;
            out[count++] = k;
        } else {
            return -1;
        }

        if (saved == '\0') break;
        *p++ = saved;
    }
    return count;
}

/* @return block_input */
int event_remapped_key_down(struct Remap * remap)
{
    if (remap->state == IDLE) {
        remap->state = HELD_DOWN_ALONE;
        remap->key_down_time = get_time_ms();
    }
    return 1;
}

/* @return block_input */
int event_remapped_key_up(struct Remap * remap)
{
    // If we receive a key UP while IDLE, it means our hook never saw the
    // matching DOWN (e.g. the OS skipped our hook for that event). Without
    // this guard we'd incorrectly fire when_alone on the orphaned UP. We
    // pass it through (return 0) so the key doesn't appear stuck, since the
    // DOWN already leaked through unblocked.
    if (remap->state == IDLE) {
        return 0;
    }

    if (remap->state == HELD_DOWN_WITH_OTHER) {
        remap->state = IDLE;
        send_sequence("with_other", remap->with_other_keys, remap->with_other_count, UP);
    } else {
        int send_alone = 1;
        unsigned long long elapsed = 0;
        int timeout_ms = (remap->timeout_ms >= 0) ? remap->timeout_ms : g_timeout_ms;
        if (timeout_ms > 0 && remap->key_down_time) {
            elapsed = get_time_ms() - remap->key_down_time;
            if (elapsed > (unsigned long long)timeout_ms) send_alone = 0;
        }
        remap->key_down_time = 0;
        remap->state = IDLE;

        if (send_alone) {
            send_sequence("when_alone", remap->when_alone_keys, remap->when_alone_count, DOWN);
            send_sequence("when_alone", remap->when_alone_keys, remap->when_alone_count, UP);
        } else if (timeout_ms > 0 && elapsed > 0) {
            if (g_debug && can_print()) {
                print_log_prefix();
                log_info("(suppressed:when_alone) held %llums > %dms", elapsed, timeout_ms);
            }
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
            remap->key_down_time = 0;
            send_sequence("with_other", remap->with_other_keys, remap->with_other_count, DOWN);
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

void cleanup_held_keys()
{
    int prev = g_paused;
    g_paused = 1;
    struct Remap * remap = g_remap_list;
    while (remap) {
        if (remap->state == HELD_DOWN_WITH_OTHER && remap->with_other_count > 0) {
            send_sequence("with_other", remap->with_other_keys, remap->with_other_count, UP);
            remap->state = IDLE;
        }
        remap = remap->next;
    }
    g_paused = prev;
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
        (g_remap_parsee->when_alone_count >= 0) &&
        (g_remap_parsee->with_other_count >= 0);
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

    // tap timeout
    if (strcmp(line, "timeout_ms") == 0) {
        int value = atoi(after_eq);
        if (value < 0) {
            log_error(
                    "Config error (line %d): timeout_ms must be >= 0\n",
                    linenum);
            return 1;
        }
        if (g_remap_parsee && (g_remap_parsee->from || g_remap_parsee->when_alone_count >= 0 || g_remap_parsee->with_other_count >= 0)) {
            g_remap_parsee->timeout_ms = value;
        } else {
            g_timeout_ms = value;
        }
        return 0;
    }

    // remap directives
    int field = 0;                  // 1 = remap_key, 2 = when_alone, 3 = with_other
    if      (strcmp(line, "remap_key")  == 0) field = 1;
    else if (strcmp(line, "when_alone") == 0) field = 2;
    else if (strcmp(line, "with_other") == 0) field = 3;

    if (field) {
        if (!g_remap_parsee)
            g_remap_parsee = new_remap(0);

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
        } else if (field == 2) {
            int count = parse_key_sequence(after_eq, g_remap_parsee->when_alone_keys);
            if (count < 0) {
                log_error(
                        "Config error (line %d): invalid key sequence '%s'\n"
                        "See the online docs for the latest list of key names.\n",
                        linenum, after_eq);
                return 1;
            }
            g_remap_parsee->when_alone_count = count;
        } else {
            int count = parse_key_sequence(after_eq, g_remap_parsee->with_other_keys);
            if (count < 0) {
                log_error(
                        "Config error (line %d): invalid key sequence '%s'\n"
                        "See the online docs for the latest list of key names.\n",
                        linenum, after_eq);
                return 1;
            }
            g_remap_parsee->with_other_count = count;
        }

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
    g_timeout_ms = 0;
}
