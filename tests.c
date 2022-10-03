#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "input.h"
#include "keys.c"
#include "remap.c"

struct Output
{
    int scan_code;
    int virt_code;
    enum Direction dir;

    struct Output * next;
};

struct Output * g_output_list = NULL;

register_output(int scan_code, int virt_code, enum Direction dir)
{
    struct Output * output = malloc(sizeof(struct Output));
    output->scan_code = scan_code;
    output->virt_code = virt_code;
    output->dir = dir;
    output->next = NULL;

    if (g_output_list) {
        struct Output * tail = g_output_list;
        while (tail->next) tail = tail->next;
        tail->next = output;
    } else {
        g_output_list = output;
    }
}

// Simulate input and pass it to our handler. If key is not swallowed, register
// it for later test inspection.
void simulate_input(int scan_code, int virt_code, enum Direction dir, int is_injected)
{
    int swallow_input = handle_input(scan_code, virt_code, dir, is_injected);
    if (!swallow_input) {
        register_output(scan_code, virt_code, dir);
    }
}

// Mock sending input through winapi
void send_input(int scan_code, int virt_code, enum Direction dir)
{
    simulate_input(scan_code, virt_code, dir, 1);
}

// Simulates user input, notably has no is_injected flag set
void user_input(int scan_code, int virt_code, enum Direction dir)
{
    simulate_input(scan_code, virt_code, dir, 0);
}

dump_outputs(char * msg)
{
    struct Output * output = g_output_list;
    printf(msg);
    int i = 0;
    while (output) {
        KEY_DEF * key = find_key_def_by_virt_code(output->virt_code);
        if (key) {
            printf("%d) %s %s\n", i, key->name, fmt_dir(output->dir));
        } else {
            printf("%d) 0x%02x %s\n", i, output->virt_code, fmt_dir(output->dir));
        }
        output = output->next;
        i++;
    }
    if (i == 0) {
        printf("<EMPTY>\n");
    }
}

// Test actions & assertions
// --------------------------------------

void IN(KEY_DEF * key, enum Direction dir)
{
    user_input(key->scan_code, key->virt_code, dir);
}

void IN_MANUAL(int scan_code, int virt_code, int dir)
{
    user_input(scan_code, virt_code, dir);
}

void SEE(KEY_DEF * key, enum Direction dir)
{
    struct Output * head = g_output_list;
    char * msg = malloc(255);
    sprintf(msg, "Expected top output to be %s %s but found:\n", key->name, fmt_dir(dir));
    if (!head) {
        dump_outputs((char *)msg);
        assert(("NOT EMPTY", head));
    }
    if (head->virt_code != key->virt_code) {
        dump_outputs((char *)msg);
        assert(("VIRT CODE", head->virt_code == key->virt_code));
    }
    if (head->dir != dir) {
        dump_outputs((char *)msg);
        assert(("OUT DIR", head->dir == dir));
    }

    g_output_list = head->next;
    head = NULL;
    free(head);
}

void EMPTY()
{
    if (!g_output_list == 0) {
        dump_outputs("Expected empty but found:\n");
        assert(("EMPTY", g_output_list == 0));
    }
}

void OK()
{
    printf("OK\n");
}

void SECTION(char * msg) {
    printf("\n");
    printf(msg);
    printf("\n----------------------------------------------\n");
}

void main()
{
    SECTION("Passthrough keys if no config");
    EMPTY();
    IN(ESC, DOWN);
    IN(SHIFT, DOWN);
    IN(ESC, UP);
        SEE(ESC, DOWN);
        SEE(SHIFT, DOWN);
        SEE(ESC, UP);
    EMPTY();
    OK();

    SECTION("Helpful error messages");
    assert(1 == load_config_line("invalid_setting=ESCAPE", 1));
    assert(1 == load_config_line("remap_key=INVALID_KEY", 2));
    assert(1 == load_config_line("remap_key::ESCAPE", 3));
    assert(0 == load_config_line("remap_key=ESCAPE", 999));
    assert(("Incomplete key", 1 == load_config_line("remap_key=ESCAPE", 4)));
    reset_config();
    OK();

    SECTION("Registers remappings from config");
    assert(("debug off by default", g_debug == 0));
    assert(0 == load_config_line("debug=1", 0));
    assert(("config turns debug on", g_debug == 1));
    assert(0 == load_config_line("debug=0", 0));
    assert(("config turns debug off", g_debug == 0));

    assert(0 == load_config_line("# Comments are ignored", 0));
    assert(0 == load_config_line("", 0));
    assert(0 == load_config_line("remap_key=CAPSLOCK", 0));
    assert(0 == load_config_line("when_alone=ESCAPE", 0));
    assert(0 == load_config_line("with_other=CTRL", 0));

    assert(0 == load_config_line("remap_key=TAB", 0));
    assert(0 == load_config_line("when_alone=TAB", 0));
    assert(0 == load_config_line("with_other=ALT", 0));

    assert(0 == load_config_line("remap_key=SHIFT", 0));
    assert(0 == load_config_line("when_alone=SPACE", 0));
    assert(0 == load_config_line("with_other=SHIFT", 0));

    assert(("registered first", g_remap_list->from == CAPS));
    assert(("registered first", g_remap_list->to_when_alone == ESC));
    assert(("registered first", g_remap_list->to_with_other == CTRL));

    assert(("registered second", g_remap_list->next->from == TAB));
    assert(("registered second", g_remap_list->next->to_when_alone == TAB));
    assert(("registered second", g_remap_list->next->to_with_other == ALT));

    assert(("registered third", g_remap_list->next->next->from == SHIFT));
    assert(("registered third", g_remap_list->next->next->to_when_alone == SPACE));
    assert(("registered third", g_remap_list->next->next->to_with_other == SHIFT));
    printf("OK\n");

    SECTION("Passthrough unmapped");
    IN(ENTER, DOWN);
        SEE(ENTER, DOWN);
    IN(ENTER, UP);
        SEE(ENTER, UP);
        EMPTY();
    OK();

    SECTION("Remap when alone");
    IN(CAPS, DOWN);
        EMPTY();
    IN(CAPS, UP);
        SEE(ESC, DOWN);
        SEE(ESC, UP);
        EMPTY();
    OK();

    SECTION("Remap with other");
    IN(CAPS, DOWN);
        EMPTY();
    IN(ENTER, DOWN);
        SEE(CTRL, DOWN);
        SEE(ENTER, DOWN);
        EMPTY();
    IN(CAPS, UP);
        SEE(CTRL, UP);
        EMPTY();
    OK();

    SECTION("Remap to self (when alone)");
    IN(TAB, DOWN);
        EMPTY();
    IN(TAB, UP);
        SEE(TAB, DOWN);
        SEE(TAB, UP);
        EMPTY();
    IN(TAB, DOWN);
    IN(ENTER, DOWN);
        SEE(ALT, DOWN);
        SEE(ENTER, DOWN);
        EMPTY();
    IN(TAB, UP);
        SEE(ALT, UP);
        EMPTY();
    OK();

    SECTION("Remap to self (with other)");
    IN(SHIFT, DOWN);
        EMPTY();
    IN(SHIFT, UP);
        SEE(SPACE, DOWN);
        SEE(SPACE, UP);
        EMPTY();
    IN(SHIFT, DOWN);
    IN(ENTER, DOWN);
        SEE(SHIFT, DOWN);
        SEE(ENTER, DOWN);
        EMPTY();
    IN(SHIFT, UP);
        SEE(SHIFT, UP);
        EMPTY();
    OK();

    SECTION("Handle duplicate same key down");
    IN(SHIFT, DOWN);
    IN(SHIFT, DOWN);
        EMPTY();
    IN(SHIFT, UP);
        SEE(SPACE, DOWN);
        SEE(SPACE, UP);
        EMPTY();
    IN(SHIFT, UP);
        SEE(SPACE, DOWN);
        SEE(SPACE, UP);
        EMPTY();
    OK();

    SECTION("Handle two remappings triggering each-other (tap first key)");
    IN(TAB, DOWN);
    IN(CAPS, DOWN);
        EMPTY();
    IN(TAB, UP);
        SEE(CTRL, DOWN);
        SEE(TAB, DOWN);
        SEE(TAB, UP);
        EMPTY();
    IN(CAPS, UP);
        SEE(CTRL, UP);
        EMPTY();
    OK();

    SECTION("Handle two remappings triggering each-other (tap second key)");
    IN(CAPS, DOWN);
    IN(TAB, DOWN);
        EMPTY();
    IN(TAB, UP);
        SEE(CTRL, DOWN);
        SEE(TAB, DOWN);
        SEE(TAB, UP);
        EMPTY();
    IN(CAPS, UP);
        SEE(CTRL, UP);
        EMPTY();
    OK();

    SECTION("Handle left/right modifier sending the same scan code");
    IN_MANUAL(SK_LEFT_SHIFT, VK_LEFT_SHIFT, DOWN);
    IN_MANUAL(SK_LEFT_SHIFT, VK_LEFT_SHIFT, UP);
        SEE(SPACE, DOWN);
        SEE(SPACE, UP);
        EMPTY();
    IN_MANUAL(SK_LEFT_SHIFT, VK_RIGHT_SHIFT, DOWN);
    IN_MANUAL(SK_LEFT_SHIFT, VK_RIGHT_SHIFT, UP);
        SEE(RSHIFT, DOWN);
        SEE(RSHIFT, UP);
        EMPTY();

    printf("\nGreat! All test passed successfully.\n");
}
