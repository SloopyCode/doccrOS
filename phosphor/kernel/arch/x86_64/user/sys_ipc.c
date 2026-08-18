/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_ipc.c
 *
 */

#include "sys_ipc.h"
#include "ptr.h"

#include <kernel/communication/ipc.h>
#include <kernel/devices/eventfd/eventfd.h>
#include <kernel/proc/process.h>

static int ipc_install_fd(proc_t *p, ipc_channel_t *channel)
{
    for (int fd = 3; fd < FD_MAX; fd++)
    {
        if (p->fd_table[fd].used) continue;

        p->fd_table[fd].node = NULL;
        p->fd_table[fd].offset = 0;
        p->fd_table[fd].used = 1;
        p->fd_table[fd].device_handle = channel;

        return fd;
    }

    return -1;
}

static ipc_channel_t *ipc_fd_channel(proc_t *p, u64 fd)
{
    if (!p) return NULL;
    if (fd < 3 || fd >= FD_MAX) return NULL;
    if (!p->fd_table[fd].used) return NULL;
    if (p->fd_table[fd].node) return NULL;

    ipc_channel_t *channel = (ipc_channel_t *)p->fd_table[fd].device_handle;

    return ipc_channel_is_valid(channel) ? channel : NULL;
}

void sys_ipc_create(cpu_state_t *s)
{
    proc_t *p = process_get_current();
    const char *name = (const char *)s->rdi;

    if (!p || !user_ptr_ok((u64)name))
    {
        s->rax = (u64)-1;
        return;
    }

    ipc_channel_t *channel = ipc_channel_create(name, p->pid);

    int fd = channel
        ? ipc_install_fd(p, channel)
        : -1
    ;

    s->rax = fd < 0 ? (u64)-1 : (u64)fd;
}

void sys_ipc_open(cpu_state_t *s)
{
    proc_t *p = process_get_current();
    const char *name = (const char *)s->rdi;

    if (!p || !user_ptr_ok((u64)name))
    {
        s->rax = (u64)-1;
        return;
    }

    ipc_channel_t *channel = ipc_channel_open(name);

    int fd = channel
        ? ipc_install_fd(p, channel)
        : -1
    ;

    s->rax = fd < 0 ? (u64)-1 : (u64)fd;
}

void sys_ipc_recv(cpu_state_t *s)
{
    void *buf = (void *)s->rsi;

    if (!user_ptr_ok((u64)buf))
    {
        s->rax = (u64)-1;
        return;
    }

    ipc_channel_t *channel = ipc_fd_channel(process_get_current(), s->rdi);
    int result = ipc_channel_recv(channel, buf, s->rdx, s->r10);

    s->rax = result < 0 ? (u64)-1 : (u64)result;
}

void sys_ipc_send(cpu_state_t *s)
{
    const void *buf = (const void *)s->rsi;

    if (!user_ptr_ok((u64)buf))
    {
        s->rax = (u64)-1;
        return;
    }

    ipc_channel_t *channel = ipc_fd_channel(process_get_current(), s->rdi);

    int result = ipc_channel_send(channel, buf, s->rdx);

    s->rax = result < 0 ? (u64)-1 : (u64)result;
}

void sys_ipc_get_eventfd(cpu_state_t *s)
{
    proc_t *p = process_get_current();
    void *handle = NULL;

    if (!p)
    {
        s->rax = (u64)-1;
        return;
    }


    ipc_channel_t *channel = ipc_fd_channel(p, s->rdi);

    if (ipc_channel_get_eventfd(channel, p->pid, &handle) < 0)
    {
        s->rax = (u64)-1;
        return;
    }

    for (int fd = 3; fd < FD_MAX; fd++)
    {
        if (p->fd_table[fd].used) continue;

        eventfd_object_t *eventfd = (eventfd_object_t *)handle;

        p->fd_table[fd].node = eventfd->node;
        p->fd_table[fd].offset = 0;
        p->fd_table[fd].used = 1;
        p->fd_table[fd].device_handle = handle;

        s->rax = (u64)fd;
        return;
    }

    eventfd_release((eventfd_object_t *)handle);


    s->rax = (u64)-1;
}