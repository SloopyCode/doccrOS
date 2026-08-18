/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_ioctl.c
 *
 */

#include "sys_ioctl.h"
#include "ptr.h"
#include "../../../fs/openfile.h"
#include <kernel/proc/process.h>
#include <kernel/fs/vfs/vfs.h>
#include <kernel/devices/device_init.h>

void sys_ioctl(cpu_state_t *state)
{
    u64 fd      = state->rdi;
    u64 request = state->rsi;
    void *arg   = (void *)state->rdx;

    if (fd >= FD_MAX)
    {
        state->rax = (u64)-1;
        return;
    }

    if (arg && !user_ptr_ok((u64)arg))
    {
        state->rax = (u64)-1;
        return;
    }

    proc_t *p = process_get_current();
    if (!p || !p->fd_table[fd].used)
    {
        state->rax = (u64)-1;
        return;
    }

    int ofd = p->fd_table[fd].ofd;
    if (ofd < 0)
    {
        state->rax = (u64)-1;
        return;
    }

    open_file_t *of = openfile_get(ofd);
    if (!of || !of->node || of->node->type != VFS_DEVICE)
    {
        state->rax = (u64)-1;
        return;
    }

    if (!of->node->device || !of->node->device->ioctl)
    {
        state->rax = (u64)-1;
        return;
    }

    state->rax = (u64)of->node->device->ioctl(
        of->device_handle,
        request,
        arg
    );
}