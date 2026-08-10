#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#  include <io.h>
#  define dup  _dup
#  define dup2 _dup2
#  define fileno _fileno
#  define close _close
#else
#  include <unistd.h>
#endif

#include "input.h"
#include "keys.c"

static unsigned long long g_fake_time_ms = 0;
static unsigned long long fake_time_ms(void) { return g_fake_time_ms; }
unsigned long long (*get_time_ms)(void) = fake_time_ms;

static int g_timer_armed_ms = 0;
static void fake_set_timer(int ms) { g_timer_armed_ms = ms; }
void (*set_timer_ms)(int ms) = fake_set_timer;

#include "remap.c"

#ifdef _WIN32
#  define DEV_NULL "NUL"
#  define DEV_TTY  "CONOUT$"
#else
#  define DEV_NULL "/dev/null"
#  define DEV_TTY  "/dev/tty"
#endif

int  g_assertions = 0, g_failures = 0;
char g_sec[128] = "";
int  g_sec_fail_at = 0;
int  g_sec_total = 0, g_sec_pass = 0;

void end_section(void)
{
    if (*g_sec) {
        int failed = g_failures - g_sec_fail_at;
        printf("%s\n", failed ? " FAIL" : " PASS");
        g_sec_total++;
        if (!failed) g_sec_pass++;
    }
}

#define SECTION(title)                                                   \
    do {                                                                 \
        end_section();                                                   \
        strncpy(g_sec, (title), sizeof g_sec - 1);                       \
        g_sec[sizeof g_sec - 1] = '\0';                                  \
        g_sec_fail_at = g_failures;                                      \
        printf("> %s", g_sec);                                           \
    } while (0)

#define EXPECT(cond, msg)                                                      \
    do {                                                                       \
        g_assertions++;                                                        \
        if (!(cond)) {                                                         \
            g_failures++;                                                      \
            printf("    FAIL %s:%d  %s\n", __FILE__, __LINE__, (msg));         \
        }                                                                      \
    } while (0)

void summary(void)
{
    end_section();
    puts("------------------------------------------------------------");
    printf("Sections: %d  Passed: %d  Failed: %d   Assertions: %d/%d\n",
           g_sec_total, g_sec_pass, g_sec_total - g_sec_pass,
           g_assertions - g_failures, g_assertions);
    printf("%s\n", g_failures ? "Some tests failed (FAIL)" : "All tests passed! (PASS)");
}

/* Output capture */
struct Output { int scan, virt;enum Direction dir;struct Output *next; };
struct Output *g_output = NULL;
void push_out(int scan,int virt,enum Direction dir){
    struct Output*n=malloc(sizeof*n);*n=(struct Output){scan,virt,dir,NULL};
    struct Output**t=&g_output;while(*t)t=&(*t)->next;*t=n;}
void clear_out(void){while(g_output){struct Output*n=g_output->next;free(g_output);g_output=n;}}

void send_input(int scan,int virt,enum Direction dir){
    if(!handle_input(scan,virt,dir,1)) push_out(scan,virt,dir);}
void user_input(int scan,int virt,enum Direction dir){
    if(!handle_input(scan,virt,dir,0)) push_out(scan,virt,dir);}

/* Platform abstraction for debug output */
int can_print(void) {
    return 1;  /* Always available in tests */
}

/* DSL */
#define IN(KEY,DIR)              user_input((KEY)->scan_code,(KEY)->virt_code,DIR)
#define IN_MANUAL(SCAN,VIRT,DIR) user_input(SCAN,VIRT,DIR)
void SEE(KEY_DEF*key,enum Direction dir){
    struct Output*n=g_output;if(!n){EXPECT(0,"output list empty");return;}
    EXPECT(n->virt==key->virt_code,"wrong virt code");
    EXPECT(n->dir==dir,"wrong direction");
    g_output=n->next;free(n);}
void EMPTY(void){EXPECT(!g_output,"output list not empty");}

/* ---- Stdout capture helpers (for debug-log tests) ---- */
static FILE *g_cap_fp = NULL;  /* temp file where we redirect stdout */
static int   g_saved_stdout = -1; /* original stdout fd */
void capture_start(void)
{
    fflush(stdout);
    g_saved_stdout = dup(fileno(stdout));
    g_cap_fp = tmpfile();
    if (g_cap_fp) {
        dup2(fileno(g_cap_fp), fileno(stdout));
    }
}
/* Returns malloc-allocated buffer with captured output (caller frees) */
char *capture_stop(void)
{
    fflush(stdout);
    long size = 0;
    if (g_cap_fp) {
        fseek(g_cap_fp, 0, SEEK_END);
        size = ftell(g_cap_fp);
        rewind(g_cap_fp);
    }
    char *buf = malloc((size_t)size + 1);
    if (size > 0 && g_cap_fp) {
        fread(buf, 1, (size_t)size, g_cap_fp);
    }
    buf[size] = '\0';
    if (g_cap_fp) fclose(g_cap_fp);
    if (g_saved_stdout != -1) {
        dup2(g_saved_stdout, fileno(stdout));
        close(g_saved_stdout);
    }
    g_cap_fp = NULL;
    g_saved_stdout = -1;
    return buf;
}

/* Tests */
int main(void)
{
    SECTION("Passthrough keys if no config");
    EMPTY(); IN(ESC,DOWN); IN(SHIFT,DOWN); IN(ESC,UP);
        SEE(ESC,DOWN); SEE(SHIFT,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("Helpful error messages");
    freopen(DEV_NULL, "w", stderr); // Silence expected error messages
    EXPECT(load_config_line("invalid_setting=ESCAPE",1)==1,"bad setting");
    EXPECT(load_config_line("remap_key=INVALID_KEY",2)==1,"bad key");
    EXPECT(load_config_line("remap_key::ESCAPE",3)==1,"bad sep");
    EXPECT(load_config_line("remap_key=ESCAPE",999)==0,"first ok");
    EXPECT(load_config_line("remap_key=ESCAPE",4)==1,"incomplete block");
    reset_config();
    freopen(DEV_TTY, "w", stderr); // Restore stderr

    SECTION("g_last_error set correctly with debug off");
    g_debug = 0;
    g_last_error[0] = '\0'; // clear
    capture_start();
    EXPECT(load_config_line("invalid_setting=ESCAPE",1)==1,"bad setting returns error");
    char *no_output = capture_stop();
    EXPECT(no_output[0] == '\0', "no stdout when debug off");
    EXPECT(strstr(g_last_error, "invalid setting") != NULL, "error stored in g_last_error");
    free(no_output);

    SECTION("g_last_error set correctly with debug on");
    g_debug = 1;
    g_last_error[0] = '\0'; // clear
    capture_start();
    EXPECT(load_config_line("remap_key=BADKEY",5)==1,"bad key returns error");
    char *with_output = capture_stop();
    EXPECT(strstr(with_output, "invalid key name") != NULL, "stdout when debug on");
    EXPECT(strstr(g_last_error, "invalid key name") != NULL, "error also stored in g_last_error");
    free(with_output);
    g_debug = 0;

    SECTION("Registers remappings from config");
    EXPECT(g_debug==0,"debug default off");
    EXPECT(load_config_line("debug=1",0)==0,"debug=1"); EXPECT(g_debug==1,"on");
    EXPECT(load_config_line("debug=0",0)==0,"debug=0"); EXPECT(g_debug==0,"off");
    EXPECT(load_config_line("# Comments are ignored",0)==0,"comment");
    EXPECT(load_config_line("",0)==0,"blank");

    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    EXPECT(load_config_line("remap_key=TAB",0)==0,"");
    EXPECT(load_config_line("when_alone=TAB",0)==0,"");
    EXPECT(load_config_line("with_other=ALT",0)==0,"");

    EXPECT(load_config_line("remap_key=SHIFT",0)==0,"");
    EXPECT(load_config_line("when_alone=SPACE",0)==0,"");
    EXPECT(load_config_line("with_other=SHIFT",0)==0,"");

    EXPECT(g_remap_list->from==CAPS,"first");
    EXPECT(g_remap_list->next->from==TAB,"second");
    EXPECT(g_remap_list->next->next->from==SHIFT,"third");

    SECTION("Passthrough unmapped");
    IN(ENTER,DOWN); SEE(ENTER,DOWN);
    IN(ENTER,UP);   SEE(ENTER,UP); EMPTY();

    SECTION("Remap when alone");
    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP); SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("Remap with other");
    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(CTRL,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("Remap to self (when alone)");
    IN(TAB,DOWN); EMPTY();
    IN(TAB,UP); SEE(TAB,DOWN); SEE(TAB,UP); EMPTY();
    IN(TAB,DOWN); IN(ENTER,DOWN); SEE(ALT,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(TAB,UP); SEE(ALT,UP); EMPTY();

    SECTION("Remap to self (with other)");
    IN(SHIFT,DOWN); EMPTY();
    IN(SHIFT,UP); SEE(SPACE,DOWN); SEE(SPACE,UP); EMPTY();
    IN(SHIFT,DOWN); IN(ENTER,DOWN); SEE(SHIFT,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(SHIFT,UP); SEE(SHIFT,UP); EMPTY();

    SECTION("Handle duplicate same key down");
    IN(SHIFT,DOWN); IN(SHIFT,DOWN); EMPTY();
    IN(SHIFT,UP); SEE(SPACE,DOWN); SEE(SPACE,UP); EMPTY();
    IN(SHIFT,UP); SEE(SHIFT,UP); EMPTY();

    SECTION("Two remappings (tap first key)");
    IN(TAB,DOWN); IN(CAPS,DOWN); EMPTY();
    IN(TAB,UP); SEE(CTRL,DOWN); SEE(TAB,DOWN); SEE(TAB,UP); EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("Two remappings (tap second key)");
    IN(CAPS,DOWN); IN(TAB,DOWN); EMPTY();
    IN(TAB,UP); SEE(CTRL,DOWN); SEE(TAB,DOWN); SEE(TAB,UP); EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("Left/right modifiers same scan");
    IN_MANUAL(SK_LEFT_SHIFT,VK_LEFT_SHIFT,DOWN);
    IN_MANUAL(SK_LEFT_SHIFT,VK_LEFT_SHIFT,UP); SEE(SPACE,DOWN); SEE(SPACE,UP); EMPTY();
    IN_MANUAL(SK_LEFT_SHIFT,VK_RIGHT_SHIFT,DOWN);
    IN_MANUAL(SK_LEFT_SHIFT,VK_RIGHT_SHIFT,UP); SEE(RSHIFT,DOWN); SEE(RSHIFT,UP); EMPTY();

    SECTION("Injected remap key is not remapped");
    EMPTY();
    send_input(CAPS->scan_code, CAPS->virt_code, DOWN);
    send_input(CAPS->scan_code, CAPS->virt_code, UP);
    SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();

    SECTION("Mouse input triggers with_other");
    IN(CAPS,DOWN); EMPTY();
    IN_MANUAL(0, MOUSE_DUMMY_VK, DOWN);
    SEE(CTRL,DOWN); SEE(MOUSE,DOWN); EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("with_other not duplicated across multiple other inputs");
    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(CTRL,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(SPACE,DOWN); SEE(SPACE,DOWN); EMPTY(); // no extra CTRL down expected
    IN(SPACE,UP); SEE(SPACE,UP); EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("Other key released before remap key");
    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(CTRL,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(ENTER,UP); SEE(ENTER,UP); EMPTY(); // CTRL still down
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("Other key held first, then remap key pressed");
    IN(ENTER,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,UP); SEE(CTRL,DOWN); SEE(ENTER,UP); EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("Two remaps cross-release");
    IN(CAPS,DOWN); IN(TAB,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(ALT,DOWN); SEE(CTRL,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();
    IN(TAB,UP); SEE(ALT,UP); EMPTY();

    SECTION("Duplicate other key down while helper held");
    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(CTRL,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(ENTER,DOWN); EMPTY(); // no second CTRL down
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("Debug logging works as expected");
    g_debug = 0;
    capture_start();
    IN(CAPS,DOWN); IN(CAPS,UP);
    char *log0 = capture_stop();
    EXPECT(log0[0] == '\0', "debug off no logs");
    free(log0);

    g_debug = 1;
    capture_start();
    IN(CAPS,DOWN);
    IN(ENTER,DOWN);
    IN(ENTER,UP);
    IN(CAPS,UP);
    IN(TAB,DOWN);
    IN(TAB,UP);
    // inputs to trigger friendly_virt_code_name fallbacks
    IN_MANUAL(0, MOUSE_DUMMY_VK, DOWN);
    IN_MANUAL(0, MOUSE_DUMMY_VK, UP);
    IN_MANUAL(0, 0, DOWN);
    IN_MANUAL(0, 0, UP);
    char *log1 = capture_stop();

    EXPECT(strstr(log1, "[input] CAPSLOCK DOWN") != NULL, "caps down logged");
    EXPECT(strstr(log1, "#blocked-input#")       != NULL, "blocked input logged");
    EXPECT(strstr(log1, "(sending:with_other)")  != NULL, "with_other send logged");
    EXPECT(strstr(log1, "(sending:when_alone)")  != NULL, "when_alone send logged");
    EXPECT(strstr(log1, "[output]")              != NULL, "output events logged");
    EXPECT(strstr(log1, "<MOUSE INPUT>")         != NULL, "mouse input friendly name");
    EXPECT(strstr(log1, "NOOP")           != NULL, "noop code friendly name");
    free(log1);

    g_debug = 0;

    // Debug section generates real outputs but doesn't consume them with SEE()
    // Drain the output queue so subsequent sections start clean
    clear_out();

    SECTION("with_other: multiple keys down in order, up in reverse");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL+ALT+SHIFT",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN);
        SEE(CTRL,DOWN); SEE(ALT,DOWN); SEE(SHIFT,DOWN);
        SEE(ENTER,DOWN);
    EMPTY();
    IN(CAPS,UP);
        SEE(SHIFT,UP); SEE(ALT,UP); SEE(CTRL,UP);
    EMPTY();

    SECTION("when_alone: combination CTRL+ESCAPE");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=CTRL+ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP);
        SEE(CTRL,DOWN); SEE(ESC,DOWN); SEE(ESC,UP); SEE(CTRL,UP);
    EMPTY();

    SECTION("with_other: case-insensitive and whitespace tolerant");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=  ctrl  +  Alt +   shift  ",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN);
        SEE(CTRL,DOWN); SEE(ALT,DOWN); SEE(SHIFT,DOWN);
        SEE(ENTER,DOWN);
    EMPTY();
    IN(CAPS,UP);
        SEE(SHIFT,UP); SEE(ALT,UP); SEE(CTRL,UP);
    EMPTY();

    SECTION("empty value acts as NOOP");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP);   EMPTY();

    SECTION("chord longer than MAX_CHORD errors");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=F1+F2+F3+F4+F5+F6+F7+F8+F9",0)==1,"too many keys must error");

    SECTION("invalid '++' sequence errors");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL++ALT",0)==1,"empty token must error");

    SECTION("timeout_ms: suppress when_alone if held too long");
    reset_config();
    EXPECT(load_config_line("timeout_ms=200",0)==0,"");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 1210;
    IN(CAPS,UP); EMPTY();

    g_fake_time_ms = 2000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 2150;
    IN(CAPS,UP); SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("timeout_ms: ignored when used with other key");
    reset_config();
    EXPECT(load_config_line("timeout_ms=100",0)==0,"");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL+ALT",0)==0,"");

    g_fake_time_ms = 3000;
    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(CTRL,DOWN); SEE(ALT,DOWN); SEE(ENTER,DOWN); EMPTY();
    g_fake_time_ms = 3700;
    IN(CAPS,UP); SEE(ALT,UP); SEE(CTRL,UP); EMPTY();

    SECTION("injected tap output must not trigger with_other");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP); SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("with_other: multi-key sequence with MOUSE input");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL+ALT",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN_MANUAL(0, MOUSE_DUMMY_VK, DOWN);
        SEE(CTRL,DOWN); SEE(ALT,DOWN); SEE(MOUSE,DOWN);
    EMPTY();
    IN(CAPS,UP);
        SEE(ALT,UP); SEE(CTRL,UP);
    EMPTY();

    SECTION("with_other: duplicate keys are kept (current behavior)");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL+CTRL+ALT",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN);
        SEE(CTRL,DOWN); SEE(CTRL,DOWN); SEE(ALT,DOWN);
        SEE(ENTER,DOWN);
    EMPTY();
    IN(CAPS,UP);
        SEE(ALT,UP); SEE(CTRL,UP); SEE(CTRL,UP);
    EMPTY();

    SECTION("Two remaps: single + multi with deterministic order");
    reset_config();
    EXPECT(load_config_line("remap_key=TAB",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=ALT+SHIFT",0)==0,"");

    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(TAB,DOWN); IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN);
        SEE(CTRL,DOWN); SEE(ALT,DOWN); SEE(SHIFT,DOWN); SEE(ENTER,DOWN);
    EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();
    IN(TAB,UP);  SEE(SHIFT,UP); SEE(ALT,UP); EMPTY();

    SECTION("with_other can include the remapped key itself (no recursion)");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL+CAPSLOCK",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN);
        SEE(CTRL,DOWN); SEE(CAPS,DOWN); SEE(ENTER,DOWN);
    EMPTY();
    IN(CAPS,UP);
        SEE(CAPS,UP); SEE(CTRL,UP);
    EMPTY();

    SECTION("timeout_ms with MOUSE as 'other'");
    reset_config();
    EXPECT(load_config_line("timeout_ms=100",0)==0,"");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL+ALT",0)==0,"");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 1150;
    IN_MANUAL(0, MOUSE_DUMMY_VK, DOWN);
        SEE(CTRL,DOWN); SEE(ALT,DOWN); SEE(MOUSE,DOWN);
    EMPTY();
    g_fake_time_ms = 1400;
    IN(CAPS,UP);
        SEE(ALT,UP); SEE(CTRL,UP);
    EMPTY();

    // Sequential steps (comma-separated)
    // ---------------------------------------------------------

    SECTION("when_alone: sequential steps ESCAPE,ESCAPE");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE,ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP);
        SEE(ESC,DOWN); SEE(ESC,UP);
        SEE(ESC,DOWN); SEE(ESC,UP);
    EMPTY();

    SECTION("when_alone: sequential chord steps CTRL+ESCAPE,CTRL+ESCAPE");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=CTRL+ESCAPE,CTRL+ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP);
        SEE(CTRL,DOWN); SEE(ESC,DOWN); SEE(ESC,UP); SEE(CTRL,UP);
        SEE(CTRL,DOWN); SEE(ESC,DOWN); SEE(ESC,UP); SEE(CTRL,UP);
    EMPTY();

    SECTION("with_other: comma rejected at parse time");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL,ALT",0)==1,"comma in with_other must error");

    SECTION("when_alone: sequential steps suppressed by timeout");
    reset_config();
    EXPECT(load_config_line("timeout_ms=200",0)==0,"");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE,ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    g_fake_time_ms = 5000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 5300;
    IN(CAPS,UP); EMPTY();

    g_fake_time_ms = 6000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 6100;
    IN(CAPS,UP);
        SEE(ESC,DOWN); SEE(ESC,UP);
        SEE(ESC,DOWN); SEE(ESC,UP);
    EMPTY();

    SECTION("when_alone: whitespace around commas");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone= ESCAPE , ESCAPE ",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP);
        SEE(ESC,DOWN); SEE(ESC,UP);
        SEE(ESC,DOWN); SEE(ESC,UP);
    EMPTY();

    SECTION("parse error: empty step between commas");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE,,ESCAPE",0)==1,"empty step must error");

    SECTION("parse error: trailing comma");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE,",0)==1,"trailing comma must error");

    // Modifier suppression (remap_key=KEY-SUPPRESS1-SUPPRESS2)
    // ---------------------------------------------------------

    KEY_DEF *F23 = find_key_def_by_name("F23");
    KEY_DEF *LWIN = find_key_def_by_name("LEFT_WIN");
    KEY_DEF *LSHIFT_KEY = find_key_def_by_name("LEFT_SHIFT");

    SECTION("suppress: with_other path");
    reset_config();
    EXPECT(load_config_line("remap_key=F23-LEFT_WIN-LEFT_SHIFT",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    // Copilot key sends WIN, SHIFT, then F23
    IN(LWIN,DOWN);    SEE(LWIN,DOWN); EMPTY();
    IN(LSHIFT_KEY,DOWN); SEE(LSHIFT_KEY,DOWN); EMPTY();
    IN(F23,DOWN);
        // suppress sends UP for WIN and SHIFT
        SEE(LWIN,UP); SEE(LSHIFT_KEY,UP);
    EMPTY();
    // now press another key -> with_other fires
    IN(ENTER,DOWN); SEE(CTRL,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(ENTER,UP);   SEE(ENTER,UP); EMPTY();
    IN(F23,UP);     SEE(CTRL,UP); EMPTY();
    // physical releases are harmless no-ops
    IN(LSHIFT_KEY,UP); SEE(LSHIFT_KEY,UP); EMPTY();
    IN(LWIN,UP);    SEE(LWIN,UP); EMPTY();

    SECTION("suppress: when_alone path with NOOP");
    reset_config();
    EXPECT(load_config_line("remap_key=F23-LEFT_WIN-LEFT_SHIFT",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(LWIN,DOWN);    SEE(LWIN,DOWN); EMPTY();
    IN(LSHIFT_KEY,DOWN); SEE(LSHIFT_KEY,DOWN); EMPTY();
    IN(F23,DOWN);     SEE(LWIN,UP); SEE(LSHIFT_KEY,UP); EMPTY();
    IN(F23,UP);       EMPTY(); // NOOP
    IN(LSHIFT_KEY,UP); SEE(LSHIFT_KEY,UP); EMPTY();
    IN(LWIN,UP);    SEE(LWIN,UP); EMPTY();

    SECTION("suppress: when_alone path with action");
    reset_config();
    EXPECT(load_config_line("remap_key=F23-LEFT_WIN-LEFT_SHIFT",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(LWIN,DOWN);    SEE(LWIN,DOWN); EMPTY();
    IN(LSHIFT_KEY,DOWN); SEE(LSHIFT_KEY,DOWN); EMPTY();
    IN(F23,DOWN);     SEE(LWIN,UP); SEE(LSHIFT_KEY,UP); EMPTY();
    IN(F23,UP);       SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("suppress: no suppress keys (backwards compat)");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP); SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("suppress: parse error invalid key");
    reset_config();
    EXPECT(load_config_line("remap_key=F23-INVALID_KEY",0)==1,"invalid suppress key must error");

    SECTION("suppress: whitespace around dashes");
    reset_config();
    EXPECT(load_config_line("remap_key=F23 - LEFT_WIN - LEFT_SHIFT",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    IN(LWIN,DOWN);    SEE(LWIN,DOWN); EMPTY();
    IN(F23,DOWN);     SEE(LWIN,UP); SEE(LSHIFT_KEY,UP); EMPTY();
    IN(F23,UP);       EMPTY();

    SECTION("suppress: parse error trailing dash");
    reset_config();
    EXPECT(load_config_line("remap_key=F23-",0)==1,"trailing dash must error");

    SECTION("suppress: parse error double dash");
    reset_config();
    EXPECT(load_config_line("remap_key=F23--LEFT_WIN",0)==1,"double dash must error");

    // Simple remap (to=KEY)
    // ---------------------------------------------------------

    SECTION("to: single key DOWN/UP");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");

    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(CAPS,UP);   SEE(ESC,UP); EMPTY();

    SECTION("to: key repeat");
    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(CAPS,UP);   SEE(ESC,UP); EMPTY();

    SECTION("to: other keys pass through without modifier");
    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(ENTER,UP);   SEE(ENTER,UP); EMPTY();
    IN(CAPS,UP);    SEE(ESC,UP); EMPTY();

    SECTION("to: orphaned UP passes through");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");

    IN(CAPS,UP); SEE(CAPS,UP); EMPTY();

    SECTION("to: chord CTRL+ALT");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=CTRL+ALT",0)==0,"");

    IN(CAPS,DOWN); SEE(CTRL,DOWN); SEE(ALT,DOWN); EMPTY();
    IN(CAPS,UP);   SEE(ALT,UP); SEE(CTRL,UP); EMPTY();

    SECTION("to: sequence fires all steps on DOWN, nothing on UP");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE,ESCAPE",0)==0,"");

    IN(CAPS,DOWN);
        SEE(ESC,DOWN); SEE(ESC,UP);
        SEE(ESC,DOWN); SEE(ESC,UP);
    EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("to: chord sequence CTRL+KEY_K,CTRL+KEY_C");
    reset_config();
    KEY_DEF *KEY_K = find_key_def_by_name("KEY_K");
    KEY_DEF *KEY_C = find_key_def_by_name("KEY_C");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=CTRL+KEY_K,CTRL+KEY_C",0)==0,"");

    IN(CAPS,DOWN);
        SEE(CTRL,DOWN); SEE(KEY_K,DOWN); SEE(KEY_K,UP); SEE(CTRL,UP);
        SEE(CTRL,DOWN); SEE(KEY_C,DOWN); SEE(KEY_C,UP); SEE(CTRL,UP);
    EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("to: sequence key repeat fires macro again");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE,ESCAPE",0)==0,"");

    IN(CAPS,DOWN);
        SEE(ESC,DOWN); SEE(ESC,UP); SEE(ESC,DOWN); SEE(ESC,UP);
    EMPTY();
    IN(CAPS,DOWN);
        SEE(ESC,DOWN); SEE(ESC,UP); SEE(ESC,DOWN); SEE(ESC,UP);
    EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("to: with suppress keys");
    reset_config();
    EXPECT(load_config_line("remap_key=F23-LEFT_WIN-LEFT_SHIFT",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");

    IN(LWIN,DOWN);       SEE(LWIN,DOWN); EMPTY();
    IN(LSHIFT_KEY,DOWN); SEE(LSHIFT_KEY,DOWN); EMPTY();
    IN(F23,DOWN);
        SEE(LWIN,UP); SEE(LSHIFT_KEY,UP);
        SEE(ESC,DOWN);
    EMPTY();
    IN(F23,DOWN); SEE(ESC,DOWN); EMPTY(); // repeat: no extra suppress
    IN(F23,UP);   SEE(ESC,UP); EMPTY();

    SECTION("to: coexists with dual remap on different key");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");
    EXPECT(load_config_line("remap_key=TAB",0)==0,"");
    EXPECT(load_config_line("when_alone=TAB",0)==0,"");
    EXPECT(load_config_line("with_other=ALT",0)==0,"");

    // simple remap works
    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(CAPS,UP);   SEE(ESC,UP); EMPTY();
    // dual remap works
    IN(TAB,DOWN); EMPTY();
    IN(TAB,UP); SEE(TAB,DOWN); SEE(TAB,UP); EMPTY();
    // both held: simple fires immediately, dual waits
    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(TAB,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(ALT,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(TAB,UP); SEE(ALT,UP); EMPTY();
    IN(CAPS,UP); SEE(ESC,UP); EMPTY();

    SECTION("to: NOOP does nothing");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=NOOP",0)==0,"");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP);   EMPTY();

    SECTION("to: case insensitive");
    reset_config();
    EXPECT(load_config_line("remap_key=capslock",0)==0,"");
    EXPECT(load_config_line("to=escape",0)==0,"");

    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(CAPS,UP);   SEE(ESC,UP); EMPTY();

    SECTION("to: mouse input does not trigger with_other");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");

    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN_MANUAL(0,MOUSE_DUMMY_VK,DOWN); SEE(MOUSE,DOWN); EMPTY();
    IN(CAPS,UP); SEE(ESC,UP); EMPTY();

    SECTION("to: two simple remaps held simultaneously");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");
    EXPECT(load_config_line("remap_key=TAB",0)==0,"");
    EXPECT(load_config_line("to=KEY_A",0)==0,"");

    KEY_DEF *KEY_A = find_key_def_by_name("KEY_A");
    IN(CAPS,DOWN); SEE(ESC,DOWN); EMPTY();
    IN(TAB,DOWN);  SEE(KEY_A,DOWN); EMPTY();
    IN(TAB,UP);    SEE(KEY_A,UP); EMPTY();
    IN(CAPS,UP);   SEE(ESC,UP); EMPTY();

    SECTION("to: parse error mixing with when_alone");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");
    EXPECT(load_config_line("when_alone=TAB",0)==1,"to + when_alone must error");

    SECTION("to: parse error mixing with with_other");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==1,"to + with_other must error");

    SECTION("to: parse error when_alone then to");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("to=CTRL",0)==1,"when_alone + to must error");

    SECTION("to: parse error invalid key");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=INVALID_KEY",0)==1,"invalid key must error");

    SECTION("parse error: directive without remap_key");
    reset_config();
    EXPECT(load_config_line("when_alone=ESCAPE",0)==1,"when_alone without remap_key");
    reset_config();
    EXPECT(load_config_line("with_other=CTRL",0)==1,"with_other without remap_key");
    reset_config();
    EXPECT(load_config_line("to=ESCAPE",0)==1,"to without remap_key");
    reset_config();
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==1,"when_timeout without remap_key");

    // Setting names are case-insensitive
    // ---------------------------------------------------------

    SECTION("setting names: case-insensitive");
    reset_config();
    EXPECT(load_config_line("Timeout_MS=200",0)==0,"Timeout_MS parses");
    EXPECT(g_timeout_ms==200,"timeout applied");
    EXPECT(load_config_line("REMAP_KEY=CAPSLOCK",0)==0,"REMAP_KEY parses");
    EXPECT(load_config_line("When_Alone=ESCAPE",0)==0,"When_Alone parses");
    EXPECT(load_config_line("WITH_OTHER=CTRL",0)==0,"WITH_OTHER parses");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 1100;
    IN(CAPS,UP); SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("setting names: Debug hits debug, not show_tray");
    EXPECT(g_show_tray==1,"show_tray default on");
    EXPECT(load_config_line("Debug=1",0)==0,"Debug=1 parses");
    EXPECT(g_debug==1,"debug set");
    EXPECT(g_show_tray==1,"show_tray untouched");
    EXPECT(load_config_line("DEBUG=0",0)==0,"DEBUG=0 parses");
    EXPECT(g_debug==0,"debug cleared");

    // when_timeout (extra action when held past timeout_ms)
    // ---------------------------------------------------------

    SECTION("when_timeout: fires immediately when timer expires");
    reset_config();
    EXPECT(load_config_line("timeout_ms=200",0)==0,"");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    EXPECT(g_timer_armed_ms==200,"timer armed for timeout");
    g_fake_time_ms = 1200;
    handle_timeout(); SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();
    IN(CAPS,UP); EMPTY(); // release swallowed, action already fired

    SECTION("when_timeout: released before timeout fires when_alone");
    g_fake_time_ms = 2000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 2100;
    IN(CAPS,UP); SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("when_timeout: fires on release if timer never fired");
    g_fake_time_ms = 3000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 3300;
    IN(CAPS,UP); SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();

    SECTION("when_timeout: with_other before timeout wins");
    g_fake_time_ms = 4000;
    IN(CAPS,DOWN); EMPTY();
    IN(ENTER,DOWN); SEE(CTRL,DOWN); SEE(ENTER,DOWN); EMPTY();
    g_fake_time_ms = 4500;
    handle_timeout(); EMPTY(); // no longer held alone, nothing fires
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("when_timeout: keys after fire pass through plain");
    g_fake_time_ms = 5000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 5250;
    handle_timeout(); SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();
    IN(ENTER,DOWN); SEE(ENTER,DOWN); EMPTY();
    IN(ENTER,UP);   SEE(ENTER,UP); EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("when_timeout: key repeat does not refire");
    g_fake_time_ms = 6000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 6250;
    handle_timeout(); SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 6900;
    handle_timeout(); EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("when_timeout: chord and sequence steps");
    reset_config();
    EXPECT(load_config_line("timeout_ms=100",0)==0,"");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("when_timeout=CTRL+ESCAPE,ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 1150;
    handle_timeout();
        SEE(CTRL,DOWN); SEE(ESC,DOWN); SEE(ESC,UP); SEE(CTRL,UP);
        SEE(ESC,DOWN); SEE(ESC,UP);
    EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("when_timeout: per-remap timeout_ms");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("timeout_ms=300",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    EXPECT(g_timer_armed_ms==300,"per-remap timeout armed");
    g_fake_time_ms = 1350;
    handle_timeout(); SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("when_timeout: without timeout_ms never fires");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 9000;
    handle_timeout(); EMPTY();
    IN(CAPS,UP); SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("when_timeout: two armed keys, nearest deadline fires first");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("timeout_ms=1000",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");
    EXPECT(load_config_line("remap_key=TAB",0)==0,"");
    EXPECT(load_config_line("timeout_ms=300",0)==0,"");
    EXPECT(load_config_line("when_alone=NOOP",0)==0,"");
    EXPECT(load_config_line("when_timeout=KEY_A",0)==0,"");
    EXPECT(load_config_line("with_other=ALT",0)==0,"");

    g_fake_time_ms = 20000;
    IN(CAPS,DOWN); EMPTY();
    EXPECT(g_timer_armed_ms==1000,"armed for caps");
    g_fake_time_ms = 20400;
    IN(TAB,DOWN); EMPTY();
    EXPECT(g_timer_armed_ms==300,"re-armed for nearer tab deadline");
    g_fake_time_ms = 20700;
    // Tab fires; its injected output counts as other input, so caps
    // converts to with_other (same rule as when_alone taps)
    handle_timeout();
        SEE(CTRL,DOWN); SEE(KEY_A,DOWN); SEE(KEY_A,UP);
    EMPTY();
    EXPECT(g_timer_armed_ms==0,"nothing left to arm");
    IN(TAB,UP); EMPTY();
    IN(CAPS,UP); SEE(CTRL,UP); EMPTY();

    SECTION("when_timeout: early release re-arms for the other key");
    g_fake_time_ms = 30000;
    IN(CAPS,DOWN); EMPTY();
    EXPECT(g_timer_armed_ms==1000,"armed for caps");
    g_fake_time_ms = 30100;
    IN(TAB,DOWN); EMPTY();
    EXPECT(g_timer_armed_ms==300,"re-armed for tab");
    g_fake_time_ms = 30200;
    IN(TAB,UP); EMPTY(); // NOOP when_alone, caps stays held alone
    g_fake_time_ms = 30400;
    handle_timeout(); EMPTY(); // stale fire for tab: nothing due yet
    EXPECT(g_timer_armed_ms==600,"re-armed for caps' remaining time");
    g_fake_time_ms = 31000;
    handle_timeout(); SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("when_timeout: stale timer fire after quick re-press");
    reset_config();
    EXPECT(load_config_line("timeout_ms=1000",0)==0,"");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");

    g_fake_time_ms = 40000;
    IN(CAPS,DOWN); EMPTY();
    EXPECT(g_timer_armed_ms==1000,"armed for first press");
    g_fake_time_ms = 40100;
    IN(CAPS,UP); SEE(ESC,DOWN); SEE(ESC,UP); EMPTY();
    g_fake_time_ms = 40200;
    IN(CAPS,DOWN); EMPTY();
    EXPECT(g_timer_armed_ms==1000,"re-armed relative to second press");
    // A stale WM_TIMER from the first press can survive KillTimer; it must
    // re-check ground truth and see only 800ms of the second hold
    g_fake_time_ms = 41000;
    handle_timeout(); EMPTY();
    EXPECT(g_timer_armed_ms==200,"re-armed for the remaining time");
    g_fake_time_ms = 41200;
    handle_timeout(); SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("when_timeout: parse error with 'to'");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==0,"");
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==1,"to + when_timeout must error");

    SECTION("when_timeout: parse error 'to' after when_timeout");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("to=ESCAPE",0)==1,"when_timeout + to must error");

    // Trailing directives (after a completed remap block)
    // ---------------------------------------------------------

    SECTION("trailing directive modifies the last registered remap");
    reset_config();
    EXPECT(load_config_line("timeout_ms=200",0)==0,"");
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");
    EXPECT(load_config_line("when_timeout=CAPSLOCK",0)==0,"when_timeout after completed block");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 1250;
    handle_timeout(); SEE(CAPS,DOWN); SEE(CAPS,UP); EMPTY();
    IN(CAPS,UP); EMPTY();

    SECTION("trailing directive: repeated directive wins");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");
    EXPECT(load_config_line("when_alone=TAB",0)==0,"overwrites earlier when_alone");

    IN(CAPS,DOWN); EMPTY();
    IN(CAPS,UP); SEE(TAB,DOWN); SEE(TAB,UP); EMPTY();

    SECTION("trailing timeout_ms is per-remap, not global");
    reset_config();
    EXPECT(load_config_line("remap_key=CAPSLOCK",0)==0,"");
    EXPECT(load_config_line("when_alone=ESCAPE",0)==0,"");
    EXPECT(load_config_line("with_other=CTRL",0)==0,"");
    EXPECT(load_config_line("timeout_ms=200",0)==0,"timeout after completed block");
    EXPECT(load_config_line("remap_key=TAB",0)==0,"");
    EXPECT(load_config_line("when_alone=TAB",0)==0,"");
    EXPECT(load_config_line("with_other=ALT",0)==0,"");
    EXPECT(g_timeout_ms==0,"global timeout untouched");

    g_fake_time_ms = 1000;
    IN(CAPS,DOWN); EMPTY();
    g_fake_time_ms = 1300;
    IN(CAPS,UP); EMPTY(); // suppressed by caps' own timeout

    g_fake_time_ms = 2000;
    IN(TAB,DOWN); EMPTY();
    g_fake_time_ms = 2300;
    IN(TAB,UP); SEE(TAB,DOWN); SEE(TAB,UP); EMPTY(); // no timeout on tab

    #include "test_keys.c"

    summary();
    clear_out();
    return g_failures ? EXIT_FAILURE : EXIT_SUCCESS;
}