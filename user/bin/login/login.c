/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: login.c
 *
 */

#include <ui16.h>
#include <libdesktop.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/input.h>

#define WIN_W 300
#define WIN_H 140

/* bit 0 of dt_event_t.modifiers, same convention s4 already uses (see s4/input/input.c) */
#define KBD_MOD_SHIFT (1 << 0)

#define FIELD_USER 0
#define FIELD_PASS 1

static char username[32] = "";
static char password[32] = "";

static int username_len = 0;
static int password_len = 0;
static int focus_field = FIELD_USER;

static unsigned int *window_buffer;

static void handle_key(unsigned int keycode, unsigned char modifiers)
{
    char *buf = (focus_field == FIELD_USER) ? username : password;
    int *len = (focus_field == FIELD_USER) ? &username_len : &password_len;
    int cap = (focus_field == FIELD_USER) ? (int)sizeof(username) - 1 : (int)sizeof(password) - 1;

    if (keycode == INPUT_KEY_BACKSPACE)
    {
        if (*len > 0) buf[--(*len)] = '\0';
        return;
    }

    if (keycode == INPUT_KEY_TAB)
    {
        focus_field = (focus_field == FIELD_USER) ? FIELD_PASS : FIELD_USER;
        return;
    }

    if (keycode == INPUT_KEY_ENTER)
    {
        if (focus_field == FIELD_USER)
        {
            focus_field = FIELD_PASS;
            return;
        }

        printf("login: user = '%s' pass = %d logged in\n", username, password_len);
        desktop.closeWindow();

        _exit(0);
    }

    char tochar = ui16_keyToChar(keycode, (modifiers & KBD_MOD_SHIFT) != 0);
    if (tochar && *len < cap)
    {
        buf[*len] = tochar;
        (*len)++;
        buf[*len] = '\0';
    }
}

static void draw_field(int field_id, const char *label, const char *value)
{
    unsigned int border_color = (focus_field == field_id) ? rgb(0, 0, 140) : rgb(0, 0, 0);

    ui16_label(style(color(rgb(0, 0, 0))), label);

    ui16_container(
        style(
            width(fill),
            height(px(22)),
            padding(4),
            bg(rgb(255, 255, 255)),
            border(1, border_color)
        )
    ) {
        ui16_label(style(color(rgb(0, 0, 0))), value);
    };
}

int main(void)
{
    int window_width;
    int window_height;
    #define APP_TITLE "login"

    desktopWindowSizeForContent(
        WIN_W,
        WIN_H,
        DT_WIN,

        &window_width,
        &window_height
    );

    desktop.createWindow(
        APP_TITLE,

        (1280 - window_width) / 2,   (720 - window_height) / 2,
        window_width,
        window_height,

        DT_WIN
    );

    window_buffer = desktop.allocFramebuffer(WIN_W, WIN_H);

    char password_stars[32];
    int i;
    int e;

    for (;;)
    {
        for (i = 0; i < password_len; i++) password_stars[i] = '*';
        password_stars[i] = '\0';

        ui16_setRoot(
            style(
                width(fill),
                height(fill),
                bg(rgb(212, 208, 200)),
                layout(column),
                padding(14),
                gap(8)
            ),
            window_buffer,

            WIN_W,
            WIN_H
        );

        draw_field(FIELD_USER, "username: ", username);
        draw_field(FIELD_PASS, "password: ", password_stars);

        ui16_frame();

        desktop.presentFrame();

        dt_event_t incoming_events[8];
        int event_count = desktop.pollEvents(incoming_events, 8);

        for (e = 0; e < event_count; e++)
        {
            dt_event_t *ev = &incoming_events[e];
            if (ev->type == DT_EV_KEY && ev->pressed) handle_key(ev->keycode, ev->modifiers);
        }

        yield();
    }

    return 0;
}