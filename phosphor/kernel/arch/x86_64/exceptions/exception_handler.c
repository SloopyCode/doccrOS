/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: exception_handler.c
 *
 */

#include "exception_handler.h"
#include "panic.h"
#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>
#include <kernel/proc/scheduler.h>
#include <kernel/communication/serial.h>

const char* exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 FPU Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

void exception_handler(cpu_state_t *state)
{
	if ((state->cs & 3) == 3)
    {
        thread_t *t = thread_get_current();
        proc_t *p   = t ? t->owner: NULL;

        const char *msg = (state->int_no < 32)
            ? exception_messages[state->int_no]
            : "Unknown Exception"
        ;

        u64 cr2 = 0;
        u64 cr3 = 0;
        if (state->int_no == 14) __asm__ volatile("mov %%cr2, %0; mov %%cr3, %1" : "=r"(cr2), "=r"(cr3));

        printf(
            "[EXC] userspace fault: '%s' int=%llu err=%llu rip=0x%llx cr2=0x%llx cr3=0x%llx proc='%s' pid=%llu ; killing process\n",
            msg,
            state->int_no,
            state->err_code,
            state->rip,
            cr2,
            cr3,
            p ? p->name : "?",
            p ? p->pid  : 0
        );

        if (t)
        {
            t->state = THREAD_DEAD;
            if (p && p->alive_count > 0) p->alive_count--;
        }

        if (p && p->state == PROC_ALIVE)
        {
            process_exit(p, 128 + (int)state->int_no);
        }

        sched_yield();

        __asm__ volatile("sti");
        for (;;) __asm__ volatile("hlt");
    }

    if (state->int_no < 32) {
        panic_exception(state, exception_messages[state->int_no]);
    } else {
        panic_exception(state, "Unknown Exception");
    }
}