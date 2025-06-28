#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "input.h"
#include "keys.c"
#include "remap.c"

static int g_assertions = 0, g_failures = 0;
static char g_sec[128] = "";
static int  g_sec_fail_at = 0;
static int  g_sec_total = 0, g_sec_pass = 0;

static void end_section(void)
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

static void summary(void)
{
    end_section();
    puts("────────────────────────────────────────────────────────────");
    printf("Sections: %d  Passed: %d  Failed: %d   Assertions: %d/%d\n",
           g_sec_total, g_sec_pass, g_sec_total - g_sec_pass,
           g_assertions - g_failures, g_assertions);
    printf("Exit status: %d\n", g_failures ? 1 : 0);
}

/* Output capture */
struct Output { int sc, vc; enum Direction dir; struct Output *next; };
static struct Output *g_out = NULL;
static void push_out(int sc,int vc,enum Direction d){
    struct Output*n=malloc(sizeof*n);*n=(struct Output){sc,vc,d,NULL};
    struct Output**t=&g_out;while(*t)t=&(*t)->next;*t=n;}
static void clear_out(void){while(g_out){struct Output*n=g_out->next;free(g_out);g_out=n;}}

void send_input(int sc,int vc,enum Direction d){
    if(!handle_input(sc,vc,d,1)) push_out(sc,vc,d);}
static void user_input(int sc,int vc,enum Direction d){
    if(!handle_input(sc,vc,d,0)) push_out(sc,vc,d);}

/* DSL */
#define IN(K,D)             user_input((K)->scan_code,(K)->virt_code,D)
#define IN_MANUAL(SC,VC,D)  user_input(SC,VC,D)
static void SEE(KEY_DEF*k,enum Direction d){
    struct Output*n=g_out;if(!n){EXPECT(0,"output list empty");return;}
    EXPECT(n->vc==k->virt_code,"wrong virt code");
    EXPECT(n->dir==d,"wrong direction");
    g_out=n->next;free(n);}
static void EMPTY(void){EXPECT(g_out==NULL,"output list not empty");}

/* Tests */
int main(void)
{
    SECTION("Passthrough keys if no config");
    EMPTY(); IN(ESC,DOWN); IN(SHIFT,DOWN); IN(ESC,UP);
        SEE(ESC,DOWN); SEE(SHIFT,DOWN); SEE(ESC,UP); EMPTY();

    SECTION("Helpful error messages");
    EXPECT(load_config_line("invalid_setting=ESCAPE",1)==1,"bad setting");
    EXPECT(load_config_line("remap_key=INVALID_KEY",2)==1,"bad key");
    EXPECT(load_config_line("remap_key::ESCAPE",3)==1,"bad sep");
    EXPECT(load_config_line("remap_key=ESCAPE",999)==0,"first ok");
    EXPECT(load_config_line("remap_key=ESCAPE",4)==1,"incomplete block");
    reset_config();

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
    IN_MANUAL(0, MOUSE_DUMMY_VK, DOWN);  // simulate mouse wheel/button event
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

    summary();
    clear_out();
    return g_failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
