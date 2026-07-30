/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: panic.c
 *
 */

#include "panic.h"
#include "../poweroff.h"
#include <kernel/screen/graphics.h>
#include <kernel/screen/colors.h>
#include <kernel/communication/serial.h>
#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

#define PANICSCREEN_COLOR 0xFFffffff
#define PANICSCREEN_BG_COLOR 0xFFff0000

__attribute__((noreturn)) void panic(const char *message)
{
	bs.Clear(BS1);
	bs.Clear(BS2);
	bs.Clear(BS3);
	bs.Clear(BS4);
    clear(PANICSCREEN_BG_COLOR);
    // Disable interrupts
    __asm__ volatile("cli");

    print("\n", PANICSCREEN_COLOR);
    print("!!! --- KERNEL PANIC --- !!!", PANICSCREEN_COLOR);
    print("\n", PANICSCREEN_COLOR);

    if (message) {
        print(message, PANICSCREEN_COLOR);
        print("\n", PANICSCREEN_COLOR);
    }

    print("\nSystem halted.", PANICSCREEN_COLOR);

    // HALT
    /*while(1) {
        __asm__ volatile("cli; hlt");
        }*/
    cpu_poweroff(POWEROFF_REBOOT);
}

__attribute__((noreturn)) void panic_exception(cpu_state_t *state, const char *message)
{
	bs.Clear(BS1);
	bs.Clear(BS2);
	bs.Clear(BS3);
	bs.Clear(BS4);
    clear(PANICSCREEN_BG_COLOR);
    // Disable interrupts
    __asm__ volatile("cli");

    print("\n", PANICSCREEN_COLOR);
    print("!!! PANIC !!!", PANICSCREEN_COLOR);
    print("\n", PANICSCREEN_COLOR);

    if (message) {
        char buf[128];
        str_copy(buf, "Exception: ");
        str_append(buf, message);
        print(buf, PANICSCREEN_COLOR);
        print("\n", PANICSCREEN_COLOR);
    }

    // Print exception details
    char buf[128];
    str_copy(buf, "INT: ");
    str_append_uint(buf, (u32)state->int_no);
    str_append(buf, " ERR: ");
    str_append_uint(buf, (u32)state->err_code);
    print(buf, PANICSCREEN_COLOR);
    print("\n", PANICSCREEN_COLOR);

    {
        char buf2[165];
        thread_t *t = thread_get_current();
        proc_t *p   = t ? t->owner: NULL;

        str_copy(buf2, "CS: 0x");
        str_from_hex(buf2 + str_len(buf2), state->cs);
        str_append(buf2, (state->cs & 3) ? "  (usermode)" : "  (kernelmode)");
        print(buf2, PANICSCREEN_COLOR);
        print("\n", PANICSCREEN_COLOR);

        str_copy(buf2, "thread: ");
        str_append(buf2, t ? t->name : "?");
        str_append(buf2, "  tid=");
        str_append_uint(buf2, t ? (u32)t->tid : 0);
        print(buf2, PANICSCREEN_COLOR);
        print("\n", PANICSCREEN_COLOR);

        str_copy(buf2, "proc: ");
        str_append(buf2, p ? p->name : "(kernel)");
        str_append(buf2, "  pid=");
        str_append_uint(buf2, p ? (u32)p->pid : 0);
        print(buf2, PANICSCREEN_COLOR);
        print("\n", PANICSCREEN_COLOR);

        printf(
            "[PANIC] cs=0x%llx (%s) thread='%s' tid=%llu proc='%s' pid=%llu\n",
            state->cs,
            (state->cs & 3) ? "user" : "kernel",
            t ? t->name : "?",
            t ? t->tid  : 0,
            p ? p->name : "(kernel)",
            p ? p->pid  : 0
        );
    }

    // page fault has VIP treatment xd
    if (state->int_no == 14) {
        u64 cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));

        str_copy(buf, "CR2 (faulting addr): 0x");
        str_from_hex(buf + str_len(buf), cr2);
        print(buf, PANICSCREEN_COLOR);
        print("\n", PANICSCREEN_COLOR);

        str_copy(buf, "  present: ");
        str_append(buf, (state->err_code & 1) ? "yes" : "no (unmapped)");
        str_append(buf, ", write: ");
        str_append(buf, (state->err_code & 2) ? "yes" : "no");
        print(buf, PANICSCREEN_COLOR);
        print("\n", PANICSCREEN_COLOR);
    }

    str_copy(buf, "RIP: 0x");
    str_from_hex(buf + str_len(buf), state->rip);
    print(buf, PANICSCREEN_COLOR);
    print("\n", PANICSCREEN_COLOR);

    // Print RSP
    str_copy(buf, "RSP: 0x");
    str_from_hex(buf + str_len(buf), state->rsp);
    print(buf, PANICSCREEN_COLOR);
    print("\n\n", PANICSCREEN_COLOR);

    print("System halted.", PANICSCREEN_COLOR);

    // HALT
    /*while(1) {
        __asm__ volatile("cli; hlt");
    }*/
    cpu_poweroff(POWEROFF_REBOOT);
}
