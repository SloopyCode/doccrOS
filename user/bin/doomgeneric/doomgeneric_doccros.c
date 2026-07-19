#include "doomgeneric.h"
#include "doomkeys.h"
#include <sys/input.h>
#include <libdesktop.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifndef DOCCROS_DOOM_SCALE
#define DOCCROS_DOOM_SCALE 2
#endif
#if DOCCROS_DOOM_SCALE < 1 || DOCCROS_DOOM_SCALE > 2
#error "DOCCROS_DOOM_SCALE must be 1 or 2"
#endif

#define DOOM_WIN_W (DOOMGENERIC_RESX * DOCCROS_DOOM_SCALE)
#define DOOM_WIN_H (DOOMGENERIC_RESY * DOCCROS_DOOM_SCALE)

static uint32_t s_winbuf[DOOM_WIN_W * DOOM_WIN_H];

static unsigned char keymap(uint16_t k) {
    static const char rows[]="1234567890-=\0\0qwertyuiop[]\0\0asdfghjkl;'`\0\\zxcvbnm,./";
    switch (k) {
        case INPUT_KEY_ESC: return KEY_ESCAPE;
        case INPUT_KEY_ENTER: case INPUT_KEY_KP_ENTER: return KEY_ENTER;
        case INPUT_KEY_TAB: return KEY_TAB;
        case INPUT_KEY_BACKSPACE: return KEY_BACKSPACE;
        /* This DoomGeneric input layer expects the old DOS translation
         * table's abstract action codes for the default fire/use keys. */
        case INPUT_KEY_LCTRL: case INPUT_KEY_RCTRL: return KEY_FIRE;
        case INPUT_KEY_LSHIFT: case INPUT_KEY_RSHIFT: return KEY_RSHIFT;
        case INPUT_KEY_LALT: case INPUT_KEY_RALT: return KEY_RALT;
        case INPUT_KEY_UP: return KEY_UPARROW;
        case INPUT_KEY_DOWN: return KEY_DOWNARROW;
        case INPUT_KEY_LEFT: return KEY_LEFTARROW;
        case INPUT_KEY_RIGHT: return KEY_RIGHTARROW;
        case INPUT_KEY_HOME: return KEY_HOME;
        case INPUT_KEY_END: return KEY_END;
        case INPUT_KEY_PAGEUP: return KEY_PGUP;
        case INPUT_KEY_PAGEDOWN: return KEY_PGDN;
        case INPUT_KEY_INSERT: return KEY_INS;
        case INPUT_KEY_DELETE: return KEY_DEL;
        case INPUT_KEY_F11: return KEY_F11;
        case INPUT_KEY_F12: return KEY_F12;
        case INPUT_KEY_SPACE: return KEY_USE;
        default: break;
    }
    if (k >= INPUT_KEY_F1 && k <= INPUT_KEY_F10)
        return (unsigned char)(KEY_F1 + k - INPUT_KEY_F1);
    if (k >= INPUT_KEY_1 && k <= INPUT_KEY_SLASH)
        return (unsigned char)rows[k - INPUT_KEY_1];
    return 0;
}

void DG_Init(void)
{
    int win_w, win_h;

    desktopWindowSizeForContent(
        DOOM_WIN_W,
        DOOM_WIN_H,
        DT_WIN,
        &win_w,
        &win_h
    );

    int rc = desktop.createWindow(
        "DOOM",
        100, 100,
        win_w, win_h,
        DT_WIN
    );

    char msg[64];
    snprintf(msg, sizeof(msg),
             "[DOOM] createWindow rc=%d content=%dx%d\\n",
             rc, DOOM_WIN_W, DOOM_WIN_H);
    write(1, msg, strlen(msg));

    memset(s_winbuf, 0, sizeof(s_winbuf));
    desktop.winbuf_write(s_winbuf, DOOM_WIN_W, DOOM_WIN_H);
}

void DG_DrawFrame(void)
{
    static uint32_t frames = 0;

    if ((frames++ % 60) == 0)
    {
        write(1, "[DOOM] frame\\n", 13);
    }

    for (uint32_t y = 0; y < DOOMGENERIC_RESY; y++) {
        uint32_t *row = &s_winbuf[(y * 2) * DOOM_WIN_W];
        const uint32_t *src = &DG_ScreenBuffer[y * DOOMGENERIC_RESX];

        for (uint32_t x = 0; x < DOOMGENERIC_RESX; x++) {
            row[x * 2]     = src[x];
            row[x * 2 + 1] = src[x];
        }

        memcpy(row + DOOM_WIN_W, row,
               DOOM_WIN_W * sizeof(uint32_t));
    }

    desktop.winbuf_write(s_winbuf, DOOM_WIN_W, DOOM_WIN_H);
}

uint32_t DG_GetTicksMs(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return(uint32_t)(t.tv_sec*1000+t.tv_nsec/1000000);}
void DG_SleepMs(uint32_t ms){uint32_t end=DG_GetTicksMs()+ms;while((int32_t)(DG_GetTicksMs()-end)<0)yield();}

int DG_GetKey(int*pressed,unsigned char*key){input_event_t e;long n=read(0,&e,sizeof e);if(n!=(long)sizeof e||e.type!=INPUT_EV_KEY)return 0;*key=keymap(e.code);if(!*key)return 0;*pressed=e.value!=0;return 1;}

void DG_SetWindowTitle(const char*t){ desktop.setTitle(t); }
int main(void){char *argv[]={"doomgeneric","-iwad","/doom1.wad",0};doomgeneric_Create(3,argv);for(;;)doomgeneric_Tick();}