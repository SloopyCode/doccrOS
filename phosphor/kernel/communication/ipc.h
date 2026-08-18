/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: ipc.c
 *
 */

#ifndef IPC_H
#define IPC_H

#include <types.h>

#define IPC_NAME_MAX 64
#define IPC_MESSAGE_MAX 256
#define IPC_QUEUE_LENGTH 64
#define IPC_NONBLOCK 1

typedef struct ipc_channel ipc_channel_t;

ipc_channel_t *ipc_channel_create(const char *name, u64 owner_pid);
ipc_channel_t *ipc_channel_open(const char *name);

int ipc_channel_send(
    ipc_channel_t *channel,
    const void *buf,
    u64 size
);

int ipc_channel_recv(
    ipc_channel_t *channel,
    void *buf,
    u64 max,
    u64 flags
);

int ipc_channel_is_valid(ipc_channel_t *channel);

int ipc_channel_get_eventfd(
    ipc_channel_t *channel,
    u64 owner_pid,
    void **out_eventfd
);

#undef  IPC_H
#endif