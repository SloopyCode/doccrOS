/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: eventfd.c
 *
 */

#include "eventfd.h"
#include <kernel/mem/meminclude.h>
#include <kernel/screen/lib/string.h>
#include <kernel/arch/hal/irqflags.h>

#define EVENTFD_MAX_OBJECTS 64

static eventfd_object_t *eventfd_registry[EVENTFD_MAX_OBJECTS];
static u64 eventfd_next_id = 1;

static int eventfd_dev_read(void *handle, void *buf, size_t count)
{
    eventfd_object_t *eventfd_object = (eventfd_object_t *)handle;
    if (!eventfd_object || !buf || count < sizeof(u64)) return -1;
    irq_state_t entry_state = irq_save();

    if (eventfd_object->closing)
    {
        irq_restore(entry_state);

        return -1;
    }
    eventfd_object->in_flight++;

    irq_restore(entry_state);

    u64 value_to_return = 0;
    int result = -1;

    for (;;)
    {
        irq_state_t saved_state = irq_save();

        if (eventfd_object->closing)
        {
            irq_restore(saved_state);

            break;
        }

        if (eventfd_object->counter != 0)
        {
            if (eventfd_object->flags & EFD_SEMAPHORE)
            {
                value_to_return = 1;
                eventfd_object->counter -= 1;
            }
            else
            {
                value_to_return = eventfd_object->counter;
                eventfd_object->counter = 0;
            }

            result = (int)sizeof(value_to_return);

            irq_restore(saved_state);
            wait_queue_wake_all(&eventfd_object->writers);

            break;
        }

        if (eventfd_object->flags & EFD_NONBLOCK)
        {
            irq_restore(saved_state);

            break;
        }

        wait_queue_block(&eventfd_object->readers);
        irq_restore(saved_state);
    }

    irq_state_t exit_state = irq_save();
    eventfd_object->in_flight--;
    int object_is_draining = eventfd_object->closing && eventfd_object->in_flight == 0;

    irq_restore(exit_state);

    if (object_is_draining) wait_queue_wake_all(&eventfd_object->drain);
    if (result < 0) return -1;

    memcpy(buf, &value_to_return, sizeof(value_to_return));
    return result;
}

static int eventfd_dev_write(void *handle, const void *buf, size_t count)
{
    eventfd_object_t *eventfd_object = (eventfd_object_t *)handle;
    if (!eventfd_object || !buf || count < sizeof(u64)) return -1;

    u64 requested_value;
    memcpy(&requested_value, buf, sizeof(requested_value));

    if (requested_value == (u64)-1) return -1;

    irq_state_t entry_state = irq_save();
    if (eventfd_object->closing)
    {
        irq_restore(entry_state);

        return -1;
    }
    eventfd_object->in_flight++;
    irq_restore(entry_state);

    int result = -1;

    for (;;)
    {
        irq_state_t saved_state = irq_save();

        if (eventfd_object->closing)
        {
            irq_restore(saved_state);

            break;
        }

        if (requested_value <= (u64)-1 - eventfd_object->counter)
        {
            eventfd_object->counter += requested_value;
            result = (int)sizeof(requested_value);

            irq_restore(saved_state);
            wait_queue_wake_all(&eventfd_object->readers);

            break;
        }

        if (eventfd_object->flags & EFD_NONBLOCK)
        {
            irq_restore(saved_state);

            break;
        }

        wait_queue_block(&eventfd_object->writers);
        irq_restore(saved_state);
    }

    irq_state_t exit_state = irq_save();
    eventfd_object->in_flight--;
    int object_is_draining = eventfd_object->closing && eventfd_object->in_flight == 0;

    irq_restore(exit_state);

    if (object_is_draining) wait_queue_wake_all(&eventfd_object->drain);

    return result;
}

static void eventfd_dev_close(void *handle)
{
    eventfd_object_t *o = handle;
    eventfd_release(o);
}

device_handler eventfd_device_ops =
{
    .name    = "eventfd",
    .mount   = NULL, // anon object
                     // isnt in fs tree
    .version = VERSION_NUM(1, 0, 0, 0),
    .init    = NULL,
    .fini    = NULL,
    .open    = NULL,
    .close   = eventfd_dev_close,
    .read    = eventfd_dev_read,
    .write   = eventfd_dev_write,
    .ioctl   = NULL,
};

eventfd_object_t *eventfd_create(u64 initial_value, u32 flags)
{
    eventfd_object_t *eventfd_object = (eventfd_object_t *)kcalloc(1, sizeof(eventfd_object_t));
    if (!eventfd_object) return NULL;

    // every eventfd gets its own lil node, nobody elses business, nobody
    // else can vfs_find their way to it either
    vfs_node_t *eventfd_node = (vfs_node_t *)kcalloc(1, sizeof(vfs_node_t));
    if (!eventfd_node)
    {
        kfree((u64 *)eventfd_object);
        return NULL;
    }

    str_copy(eventfd_node->name, "eventfd");
    eventfd_node->type = VFS_DEVICE;
    eventfd_node->device = &eventfd_device_ops;

    eventfd_object->counter  = initial_value;
    eventfd_object->flags = flags;
    eventfd_object->node  = eventfd_node;
    eventfd_object->refcount = 1;

    wait_queue_init(&eventfd_object->readers);
    wait_queue_init(&eventfd_object->writers);
    wait_queue_init(&eventfd_object->drain);

    irq_state_t saved_state = irq_save();

    int slot = -1;
    for (int i = 0; i < EVENTFD_MAX_OBJECTS; i++)
    {
        if (!eventfd_registry[i])
        {
            slot = i;
            break;
        }
    }

    if (slot >= 0)
    {
        eventfd_object->id = eventfd_next_id++;

        eventfd_registry[slot] = eventfd_object;
    }
    else
    {
        eventfd_object->id = 0;
    }

    irq_restore(saved_state);

    return eventfd_object;
}

eventfd_object_t *eventfd_find(u64 id)
{
    if (id == 0) return NULL;

    irq_state_t saved_state = irq_save();
    eventfd_object_t *found = NULL;

    for (int i = 0; i < EVENTFD_MAX_OBJECTS; i++)
    {
        if (eventfd_registry[i] && eventfd_registry[i]->id == id)
        {
            found = eventfd_registry[i];
            break;
        }
    }

    irq_restore(saved_state);
    return found;
}

eventfd_object_t *eventfd_acquire(u64 id)
{
    eventfd_object_t *eventfd_object = eventfd_find(id);
    if (!eventfd_object) return NULL;

    irq_state_t saved_state = irq_save();

    if (eventfd_object->closing)
    {
        irq_restore(saved_state);
        return NULL;
    }

    eventfd_object->refcount++;

    irq_restore(saved_state);

    return eventfd_object;
}

void eventfd_release(eventfd_object_t  *eventfd_object)
{
    if (!eventfd_object) return;
    if (--eventfd_object->refcount <= 0) eventfd_destroy(eventfd_object);
}

void eventfd_signal(eventfd_object_t *eventfd_object, u64 value)
{
    if (!eventfd_object || value == 0 || value == (u64)-1) return;

    irq_state_t state = irq_save();
    int signal = !eventfd_object->closing && value <= (u64)-1 - eventfd_object->counter;
    if (signal) eventfd_object->counter += value;

    irq_restore(state);

    if (signal) wait_queue_wake_all(&eventfd_object->readers);
}

void eventfd_destroy(eventfd_object_t *eventfd_object)
{
    if (!eventfd_object) return;

    irq_state_t saved_state = irq_save();
    eventfd_object->closing = 1;

    irq_restore(saved_state);
    wait_queue_wake_all(&eventfd_object->readers);
    wait_queue_wake_all(&eventfd_object->writers);

    for (;;)
    {
        irq_state_t drain_state = irq_save();

        if (eventfd_object->in_flight == 0)
        {
            irq_restore(drain_state);
            break;
        }

        wait_queue_block(&eventfd_object->drain);
        irq_restore(drain_state);
    }

    irq_state_t reg_state = irq_save();

    for (int i = 0; i < EVENTFD_MAX_OBJECTS; i++)
    {
        if (eventfd_registry[i] == eventfd_object)
        {
            eventfd_registry[i] = NULL;
            break;
        }
    }

    irq_restore(reg_state);

    kfree((u64 *)eventfd_object->node);
    kfree((u64 *)eventfd_object);
}
