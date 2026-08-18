/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: ipc.c
 *
 */

#include "ipc.h"

#include <kernel/arch/hal/irqflags.h>
#include <kernel/devices/eventfd/eventfd.h>
#include <kernel/mem/meminclude.h>
#include <kernel/screen/lib/string.h>
#include <kernel/proc/wait.h>

//NOTE:
// this ipc is just here as test
// i need a simple ipc for the desktop and dont want to
// implement a complex unix like socket ips thing or whatever.
// but ofc in future this will be replaced with a proper ipc

#define IPC_MAX_CHANNELS 64
#define IPC_CHANNEL_MAGIC 0x49504343U

typedef struct
{
    u16 size;
    u8 data[IPC_MESSAGE_MAX];
} ipc_message_t;

struct ipc_channel
{
    u32 magic;
    char name[IPC_NAME_MAX];

    u64 owner_pid;

    u32 head;
    u32 tail;
    u32 count;

    ipc_message_t messages[IPC_QUEUE_LENGTH];

    eventfd_object_t *notify;
    wait_queue_t readers;
};

static ipc_channel_t *channels[IPC_MAX_CHANNELS];

int ipc_channel_is_valid(ipc_channel_t *channel)
{
    return channel && channel->magic == IPC_CHANNEL_MAGIC;
}

static ipc_channel_t *ipc_channel_find_locked(const char *name)
{
    for (int i = 0; i < IPC_MAX_CHANNELS; i++)
    {
        ipc_channel_t *channel = channels[i];

        if (!channel) continue;
        if (str_equals(channel->name, name)) return channel;
    }

    return NULL;
}

ipc_channel_t *ipc_channel_open(const char *name)
{
    if (!name || !name[0]) return NULL;

    irq_state_t state = irq_save();

    ipc_channel_t *channel = ipc_channel_find_locked(name);

    irq_restore(state);

    return channel;
}

ipc_channel_t *ipc_channel_create(const char *name, u64 owner_pid)
{
    if (!name || !name[0] || !owner_pid) return NULL;

    irq_state_t state = irq_save();

    ipc_channel_t *existing = ipc_channel_find_locked(name);
    int slot = -1;

    for (int i = 0; i < IPC_MAX_CHANNELS; i++)
    {
        if (!channels[i])
        {
            slot = i;
            break;
        }
    }

    irq_restore(state);

    if (existing) return existing;
    if (slot < 0) return NULL;

    ipc_channel_t *channel = (ipc_channel_t *)kcalloc(1, sizeof(*channel));

    if (!channel) return NULL;

    channel->notify = eventfd_create(0, 0);

    if (!channel->notify)
    {
        kfree((u64 *)channel);
        return NULL;
    }

    channel->magic = IPC_CHANNEL_MAGIC;
    channel->owner_pid = owner_pid;

    str_copy(channel->name, name);
    wait_queue_init(&channel->readers);

    state = irq_save();

    existing = ipc_channel_find_locked(name);

    if (!existing) channels[slot] = channel;

    irq_restore(state);

    if (existing)
    {
        eventfd_release(channel->notify);

        kfree((u64 *)channel);

        return existing;
    }

    return channel;
}

int ipc_channel_send(ipc_channel_t *channel, const void *buf, u64 size)
{
    if (!ipc_channel_is_valid(channel)) return -1;
    if (!buf || size == 0 || size > IPC_MESSAGE_MAX) return -1;

    irq_state_t state = irq_save();

    if (channel->count >= IPC_QUEUE_LENGTH)
    {
        irq_restore(state);

        return -1;
    }

    ipc_message_t *message = &channel->messages[channel->head];

    message->size = (u16)size;
    memcpy(message->data, buf, size);

    channel->head = (channel->head + 1) % IPC_QUEUE_LENGTH;
    channel->count++;

    irq_restore(state);

    eventfd_signal(channel->notify, 1);

    wait_queue_wake_one(&channel->readers);

    return (int)size;
}

int ipc_channel_recv(ipc_channel_t *channel, void *buf, u64 max, u64 flags)
{
    if (!ipc_channel_is_valid(channel) || !buf) return -1;

    for (;;)
    {
        irq_state_t state = irq_save();

        if (channel->count > 0)
        {
            ipc_message_t *message = &channel->messages[channel->tail];

            if (max < message->size)
            {
                irq_restore(state);
                return -1;
            }

            u64 size = message->size;

            memcpy(buf, message->data, size);

            channel->tail = (channel->tail + 1) % IPC_QUEUE_LENGTH;
            channel->count--;

            irq_restore(state);

            return (int)size;
        }

        if (flags & IPC_NONBLOCK)
        {
            irq_restore(state);

            return 0;
        }

        wait_queue_block(&channel->readers);

        irq_restore(state);
    }
}

int ipc_channel_get_eventfd(
    ipc_channel_t *channel,
    u64 owner_pid,
    void **out_eventfd)
{
    if (!ipc_channel_is_valid(channel)) return -1;
    if (!out_eventfd || channel->owner_pid != owner_pid) return -1;

    eventfd_object_t *object = eventfd_acquire(channel->notify->id);

    if (!object) return -1;

    *out_eventfd = object;

    return 0;
}