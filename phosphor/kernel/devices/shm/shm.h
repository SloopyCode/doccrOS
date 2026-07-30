/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: shm.h
 *
 */

#ifndef DEVICES_SHM_H
#define DEVICES_SHM_H

#include <types.h>
#include <kernel/devices/device_init.h>

#define SHM_IOCTL_ALLOC     0
#define SHM_IOCTL_MAP       1
#define SHM_IOCTL_UNMAP     2
#define SHM_IOCTL_GET_SIZE  3

typedef struct
{
    u64     id;
    u64     size;
    u64     vaddr;
} shm_ioctl_args_t;

extern device_handler shm_device;

#endif