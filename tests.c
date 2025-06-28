#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#ifdef _WIN32
#  include <io.h>
#  define dup  _dup
#  define dup2 _dup2
#  define fileno _fileno
#endif

#include "input.h"
#include "keys.c"
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
        printf("%s\n", failed ? "" : " ✔");
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
    puts("────────────────────────────────────────────────────────────");
    printf("Sections: %d  Passed: %d  Failed: %d   Assertions: %d/%d\n",
           g_sec_total, g_sec_pass, g_sec_total - g_sec_pass,
           g_assertions - g_failures, g_assertions);
    printf("%s\n", g_failures ? "Some tests failed ✘" : "All tests passed! ✔");
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
    IN(SHIFT,UP); SEE(SPACE,DOWN); SEE(SPACE,UP); EMPTY();

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
    EXPECT(strstr(log1, "<ZERO_CODE>")           != NULL, "zero code friendly name");
    free(log1);

    g_debug = 0;

    summary();
    clear_out();
    return g_failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
