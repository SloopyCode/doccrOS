/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sipc.c
 *
 */

#include "syscall.h"

#include <sys/sipc.h>

long ipc_create(const char *name)
{
    return syscall3(SYS_IPC_CREATE, (long)name, 0, 0);
}

long ipc_open(const char *name)
{
    return syscall3(SYS_IPC_OPEN, (long)name, 0, 0);
}

long ipc_send(int fd, const void *buf, size_t count)
{
    return syscall3(SYS_IPC_SEND, fd, (long)buf, (long)count);
}

long ipc_recv(int fd, void *buf, size_t count, int flags)
{
    return syscall6(SYS_IPC_RECV, fd, (long)buf, (long)count, flags, 0, 0);
}

long ipc_eventfd(int fd)
{
    return syscall3(SYS_IPC_EVENTFD, fd, 0, 0);
}

