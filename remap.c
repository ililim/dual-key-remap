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

#define MAX_CHORD 8
#define MAX_STEPS 8

enum State {
    IDLE,
    HELD_DOWN_ALONE,
    HELD_DOWN_WITH_OTHER,
    HELD_DOWN_TIMED_OUT,
};

struct Step {
    KEY_DEF * keys[MAX_CHORD];
    int count;
};

struct Remap
{
    KEY_DEF * from;

    KEY_DEF * suppress[MAX_CHORD];
    int suppress_count;

    struct Step when_alone[MAX_STEPS];
    int when_alone_steps;

    struct Step with_other;
    int with_other_steps;

    struct Step when_timeout[MAX_STEPS];
    int when_timeout_steps;

    struct Step to[MAX_STEPS];
    int to_steps;

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

// Time (provided by host: dual-key-remap.c or tests.c)
extern unsigned long long (*get_time_ms)(void);

// One-shot timer (provided by host); calls handle_timeout after ms, 0 cancels
extern void (*set_timer_ms)(int ms);

// Debug Logging
// --------------------------------------

extern int can_print(void);

#define log_error(fmt, ...) do { \
    if (g_debug && can_print()) { \
        printf(fmt, ##__VA_ARGS__); \
    } \
    snprintf(g_last_error, sizeof(g_last_error), fmt, ##__VA_ARGS__); \
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

struct Remap * new_remap()
{
    struct Remap * remap = calloc(1, sizeof(struct Remap));
    remap->state = IDLE;
    remap->when_alone_steps = -1;
    remap->with_other_steps = -1;
    remap->when_timeout_steps = -1;
    remap->to_steps = -1;
    remap->timeout_ms = -1;
    return remap;
}

struct Remap * last_remap()
{
    struct Remap * remap = g_remap_list;
    while (remap && remap->next) remap = remap->next;
    return remap;
}

void register_remap(struct Remap * remap)
{
    struct Remap * tail = last_remap();
    if (tail) tail->next = remap;
    else g_remap_list = remap;
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

static void send_chord(const char *name, KEY_DEF **keys, int count, enum Direction dir)
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

static void send_steps(const char *name, struct Step *steps, int count)
{
    for (int i = 0; i < count; i++) {
        send_chord(name, steps[i].keys, steps[i].count, DOWN);
        send_chord(name, steps[i].keys, steps[i].count, UP);
    }
}

static int effective_timeout_ms(struct Remap *remap)
{
    return (remap->timeout_ms >= 0) ? remap->timeout_ms : g_timeout_ms;
}

// Arm the host timer for the nearest pending when_timeout deadline, or cancel it when none
static void arm_timeout_timer()
{
    unsigned long long now = get_time_ms();
    int nearest = 0;
    for (struct Remap *remap = g_remap_list; remap; remap = remap->next) {
        if (remap->state != HELD_DOWN_ALONE || remap->when_timeout_steps <= 0 || !remap->key_down_time)
            continue;
        int timeout_ms = effective_timeout_ms(remap);
        if (timeout_ms <= 0) continue;
        unsigned long long deadline = remap->key_down_time + timeout_ms;
        int remaining = (deadline > now) ? (int)(deadline - now) : 1;
        if (!nearest || remaining < nearest) nearest = remaining;
    }
    set_timer_ms(nearest);
}

static int parse_chord(char *value, KEY_DEF **out)
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
            if (count >= MAX_CHORD) return -1;
            out[count++] = k;
        } else {
            return -1;
        }

        if (saved == '\0') break;
        *p++ = saved;
    }
    return count;
}

static int parse_steps(char *value, struct Step *out, int max_steps)
{
    while (*value && isspace((unsigned char)*value)) ++value;
    if (*value == '\0') return 0;
    if (strcasecmp(value, "NOOP") == 0) return 0;

    int step_count = 0;
    char *p = value;
    while (*p) {
        while (isspace((unsigned char)*p)) ++p;
        if (*p == '\0') break;

        char *start = p;
        while (*p && *p != ',') ++p;

        char saved = *p;
        *p = '\0';

        if (step_count >= max_steps) return -1;
        int count = parse_chord(start, out[step_count].keys);
        if (count <= 0) return -1;
        out[step_count].count = count;
        step_count++;

        if (saved == '\0') break;
        *p++ = saved;

        // After a comma, the next step must not be empty
        while (isspace((unsigned char)*p)) ++p;
        if (*p == '\0' || *p == ',') return -1;
    }
    return step_count;
}

/* @return block_input */
int event_remapped_key_down(struct Remap * remap)
{
    // Simple remap: send replacement immediately, bypass state machine
    if (remap->to_steps > 0) {
        if (remap->state == IDLE) {
            for (int i = 0; i < remap->suppress_count; i++)
                send_input(remap->suppress[i]->scan_code,
                           remap->suppress[i]->virt_code, UP);
            remap->state = HELD_DOWN_ALONE;
        }
        // Single step: held substitution (DOWN on press, UP on release)
        // Multi step: macro (all steps fire on each press, nothing on release)
        if (remap->to_steps == 1) {
            send_chord("to", remap->to[0].keys, remap->to[0].count, DOWN);
        } else {
            send_steps("to", remap->to, remap->to_steps);
        }
        return 1;
    }

    if (remap->state == IDLE) {
        // Release suppressed modifiers before changing state, otherwise
        // the injected UPs would trigger event_other_input for this remap
        for (int i = 0; i < remap->suppress_count; i++) {
            send_input(remap->suppress[i]->scan_code,
                       remap->suppress[i]->virt_code, UP);
        }
        remap->state = HELD_DOWN_ALONE;
        remap->key_down_time = get_time_ms();
        if (remap->when_timeout_steps > 0) arm_timeout_timer();
    }
    return 1;
}

/* @return block_input */
int event_remapped_key_up(struct Remap * remap)
{
    // Simple remap: release replacement, bypass state machine
    if (remap->to_steps > 0) {
        if (remap->state == IDLE)
            return 0; // orphaned UP: DOWN leaked through, pass UP so key doesn't stick
        remap->state = IDLE;
        if (remap->to_steps == 1)
            send_chord("to", remap->to[0].keys, remap->to[0].count, UP);
        // multi-step: nothing on UP (all steps fired on DOWN)
        return 1;
    }

    // If we receive a key UP while IDLE, it means our hook never saw the
    // matching DOWN (e.g. the OS skipped our hook for that event). Without
    // this guard we'd incorrectly fire when_alone on the orphaned UP. We
    // pass it through (return 0) so the key doesn't appear stuck, since the
    // DOWN already leaked through unblocked.
    if (remap->state == IDLE) {
        return 0;
    }

    // when_timeout already fired while held; swallow the release
    if (remap->state == HELD_DOWN_TIMED_OUT) {
        remap->state = IDLE;
        return 1;
    }

    if (remap->state == HELD_DOWN_WITH_OTHER) {
        remap->state = IDLE;
        send_chord("with_other", remap->with_other.keys, remap->with_other.count, UP);
    } else {
        int send_alone = 1;
        unsigned long long elapsed = 0;
        int timeout_ms = effective_timeout_ms(remap);
        if (timeout_ms > 0 && remap->key_down_time) {
            elapsed = get_time_ms() - remap->key_down_time;
            if (elapsed >= (unsigned long long)timeout_ms) send_alone = 0;
        }
        remap->key_down_time = 0;
        remap->state = IDLE;

        if (send_alone) {
            send_steps("when_alone", remap->when_alone, remap->when_alone_steps);
        } else if (remap->when_timeout_steps > 0) {
            // Timer missed the deadline (host lag)
            send_steps("when_timeout", remap->when_timeout, remap->when_timeout_steps);
        } else if (timeout_ms > 0 && elapsed > 0) {
            if (g_debug && can_print()) {
                print_log_prefix();
                log_info("(suppressed:when_alone) held %llums >= %dms", elapsed, timeout_ms);
            }
        }
    }
    return 1;
}

/* @return block_input */
int event_other_input()
{
    struct Remap * remap = g_remap_list;
    while (remap) {
        if (remap->to_steps <= 0 && remap->state == HELD_DOWN_ALONE) {
            remap->state = HELD_DOWN_WITH_OTHER;
            remap->key_down_time = 0;
            send_chord("with_other", remap->with_other.keys, remap->with_other.count, DOWN);
        }
        remap = remap->next;
    }
    return 0;
}

// Called by the host when the timeout timer fires
void handle_timeout()
{
    if (g_paused) return;

    unsigned long long now = get_time_ms();
    for (struct Remap *remap = g_remap_list; remap; remap = remap->next) {
        if (remap->state != HELD_DOWN_ALONE || remap->when_timeout_steps <= 0 || !remap->key_down_time)
            continue;
        int timeout_ms = effective_timeout_ms(remap);
        if (timeout_ms <= 0) continue;
        if (now - remap->key_down_time < (unsigned long long)timeout_ms) continue;
        // Change state before sending, otherwise our own injected output
        // would trigger event_other_input for this remap
        remap->state = HELD_DOWN_TIMED_OUT;
        remap->key_down_time = 0;
        send_steps("when_timeout", remap->when_timeout, remap->when_timeout_steps);
    }
    arm_timeout_timer();
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
        if (remap->state != IDLE && remap->to_steps == 1) {
            send_chord("to", remap->to[0].keys, remap->to[0].count, UP);
            remap->state = IDLE;
        } else if (remap->state == HELD_DOWN_WITH_OTHER && remap->with_other.count > 0) {
            send_chord("with_other", remap->with_other.keys, remap->with_other.count, UP);
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
    if (!g_remap_parsee || !g_remap_parsee->from) return 0;
    if (g_remap_parsee->to_steps >= 0) return 1;
    return (g_remap_parsee->when_alone_steps >= 0) &&
           (g_remap_parsee->with_other_steps >= 0);
}

static int parse_bool(const char *value) {
    if (strcmp(value, "1") == 0 || strcasecmp(value, "true")  == 0) return 1;
    if (strcmp(value, "0") == 0 || strcasecmp(value, "false") == 0) return 0;
    return -1;
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

    // setting names are case-insensitive
    for (char *c = line; *c; c++) *c = tolower((unsigned char)*c);

    // boolean settings
    if (strcmp(line, "debug") == 0 || strcmp(line, "show_tray") == 0) {
        int v = parse_bool(after_eq);
        if (v < 0) {
            log_error("Config error (line %d): %s must be 0/1/true/false\n",
                    linenum, line);
            return 1;
        }
        if (line[0] == 'd') g_debug = v; else g_show_tray = v;
        return 0;
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
        // Global when set before any remap, per-remap after
        struct Remap *target = g_remap_parsee ? g_remap_parsee : last_remap();
        if (target) target->timeout_ms = value;
        else g_timeout_ms = value;
        return 0;
    }

    // remap directives
    int field = 0;                  // 1 = remap_key, 2 = when_alone, 3 = with_other, 4 = to, 5 = when_timeout
    if      (strcmp(line, "remap_key")    == 0) field = 1;
    else if (strcmp(line, "when_alone")   == 0) field = 2;
    else if (strcmp(line, "with_other")   == 0) field = 3;
    else if (strcmp(line, "to")           == 0) field = 4;
    else if (strcmp(line, "when_timeout") == 0) field = 5;

    if (field) {
        if (field == 1) {
            if (g_remap_parsee && g_remap_parsee->from && !parsee_is_valid()) {
                log_error(
                        "Config error (line %d): Incomplete remapping.\n"
                        "Each remap needs remap_key with either 'to' or\n"
                        "'when_alone' and 'with_other' before another remap_key.\n",
                        linenum);
                return 1;
            }
            free(g_remap_parsee);
            g_remap_parsee = new_remap();
            // Parse remap_key: KEY or KEY-SUPPRESS1-SUPPRESS2
            char *dash = strchr(after_eq, '-');
            if (dash) *dash = '\0';

            // right-trim key name (needed when spaces surround the dash)
            {
                size_t klen = strlen(after_eq);
                while (klen > 0 && isspace((unsigned char)after_eq[klen-1]))
                    after_eq[--klen] = '\0';
            }

            for (char *c = after_eq; *c; c++) *c = toupper((unsigned char)*c);
            KEY_DEF *key_def = find_key_def_by_name(after_eq);
            if (!key_def) {
                log_error(
                        "Config error (line %d): invalid key name '%s'\n"
                        "See the online docs for the latest list of key names.\n",
                        linenum, after_eq);
                return 1;
            }
            g_remap_parsee->from = key_def;

            if (dash) {
                char *p = dash + 1;
                while (isspace((unsigned char)*p)) ++p;
                if (*p == '\0' || *p == '-') {
                    log_error("Config error (line %d): empty suppress key\n", linenum);
                    return 1;
                }
                while (*p) {
                    while (isspace((unsigned char)*p)) ++p;
                    if (*p == '\0') break;
                    char *start = p;
                    while (*p && *p != '-') ++p;
                    char saved = *p;
                    *p = '\0';

                    size_t len = strlen(start);
                    while (len > 0 && isspace((unsigned char)start[len-1]))
                        start[--len] = '\0';

                    if (len == 0) {
                        log_error("Config error (line %d): empty suppress key\n", linenum);
                        return 1;
                    }

                    for (char *c = start; *c; c++) *c = toupper((unsigned char)*c);
                    KEY_DEF *sk = find_key_def_by_name(start);
                    if (!sk) {
                        log_error(
                                "Config error (line %d): invalid suppress key '%s'\n"
                                "See the online docs for the latest list of key names.\n",
                                linenum, start);
                        return 1;
                    }
                    if (g_remap_parsee->suppress_count >= MAX_CHORD) {
                        log_error("Config error (line %d): too many suppress keys\n", linenum);
                        return 1;
                    }
                    g_remap_parsee->suppress[g_remap_parsee->suppress_count++] = sk;

                    if (saved == '\0') break;
                    *p++ = saved;
                }
            }
        } else {
            // Directives modify the remap being parsed, or the last registered
            // one, so e.g. when_timeout can follow a completed block
            struct Remap *target = g_remap_parsee ? g_remap_parsee : last_remap();
            if (!target) {
                log_error(
                        "Config error (line %d): '%s' must come after 'remap_key'\n",
                        linenum, line);
                return 1;
            }
            if (field == 2 || field == 3 || field == 5) {
                if (target->to_steps >= 0) {
                    log_error(
                            "Config error (line %d): cannot use '%s' with 'to'\n",
                            linenum, line);
                    return 1;
                }
                if (field == 2 || field == 5) {
                    struct Step *steps = (field == 2)
                        ? target->when_alone
                        : target->when_timeout;
                    int *step_count = (field == 2)
                        ? &target->when_alone_steps
                        : &target->when_timeout_steps;
                    int count = parse_steps(after_eq, steps, MAX_STEPS);
                    if (count < 0) {
                        log_error(
                                "Config error (line %d): invalid key sequence '%s'\n"
                                "See the online docs for the latest list of key names.\n",
                                linenum, after_eq);
                        return 1;
                    }
                    *step_count = count;
                } else {
                    if (strchr(after_eq, ',')) {
                        log_error(
                                "Config error (line %d): with_other does not support"
                                " comma-separated steps\n", linenum);
                        return 1;
                    }
                    int count = parse_steps(after_eq, &target->with_other, 1);
                    if (count < 0) {
                        log_error(
                                "Config error (line %d): invalid key sequence '%s'\n"
                                "See the online docs for the latest list of key names.\n",
                                linenum, after_eq);
                        return 1;
                    }
                    target->with_other_steps = count;
                }
            } else {
                if (target->when_alone_steps >= 0 ||
                    target->with_other_steps >= 0 ||
                    target->when_timeout_steps >= 0) {
                    log_error(
                            "Config error (line %d): cannot use 'to' with"
                            " 'when_alone'/'with_other'/'when_timeout'\n", linenum);
                    return 1;
                }
                int count = parse_steps(after_eq, target->to, MAX_STEPS);
                if (count < 0) {
                    log_error(
                            "Config error (line %d): invalid key sequence '%s'\n"
                            "See the online docs for the latest list of key names.\n",
                            linenum, after_eq);
                    return 1;
                }
                target->to_steps = count;
            }
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
