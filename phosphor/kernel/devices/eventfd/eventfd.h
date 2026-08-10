/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: eventfd.h
 *
 */

#ifndef DEVICES_EVENTFD_H
#define DEVICES_EVENTFD_H

#include <types.h>
#include <kernel/devices/device_init.h>
#include <kernel/fs/vfs/vfs.h>
#include <kernel/proc/wait.h>

// same as in userspace libc otherwise it wont work
#define EFD_SEMAPHORE 0x00000001
#define EFD_NONBLOCK  0x00000800

typedef struct
{
    u64    counter;
    u32    flags;
    int    in_flight;
    int    closing;
    int    refcount;

    wait_queue_t   readers;
    wait_queue_t   writers;
    wait_queue_t   drain;
    vfs_node_t     *node;
} eventfd_object_t;

extern device_handler eventfd_device_ops;

eventfd_object_t *eventfd_create(u64 initial_value, u32 flags);
void eventfd_destroy(eventfd_object_t *eventfd_object);

#endif