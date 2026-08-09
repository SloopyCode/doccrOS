/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_eventfd.c
 *
 */

#include "sys_eventfd.h"
#include <kernel/proc/process.h>
#include <kernel/devices/eventfd/eventfd.h>

void sys_eventfd(cpu_state_t *state)
{
    u32 initial_value = (u32)state->rdi;
    u32 eventfd_flags = (u32)state->rsi;

    if (eventfd_flags & ~(u32)(EFD_NONBLOCK | EFD_SEMAPHORE))
    {
        state->rax  = (u64)-1;

        return;
    }

    proc_t *process = process_get_current();
    if (!process)
    {
        state->rax  = (u64)-1;

        return;
    }

    eventfd_object_t *eventfd_object = eventfd_create((u64)initial_value, eventfd_flags);
    if (!eventfd_object)
    {
        state->rax  = (u64)-1;

        return;
    }

    for (int file_descriptor = 3; file_descriptor < FD_MAX; file_descriptor++)
    {
        if (!process->fd_table[file_descriptor].used)
        {
            process->fd_table[file_descriptor].node   = eventfd_object->node;
            process->fd_table[file_descriptor].offset = 0;
            process->fd_table[file_descriptor].used   = 1;
            process->fd_table[file_descriptor].device_handle = eventfd_object;

            state->rax = (u64)file_descriptor;
            return;
        }
    }

    // fd table already full
    eventfd_destroy(eventfd_object);
    state->rax = (u64)-1;
}