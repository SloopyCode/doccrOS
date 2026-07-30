/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys/shm.h
 *
 */

#pragma once
#include <stdint.h>

#define SHM_DEV "/dev/shm0"

#define SHM_IOCTL_ALLOC 0
#define SHM_IOCTL_MAP   1
#define SHM_IOCTL_UNMAP 2
#define SHM_IOCTL_GET_SIZE 3

typedef struct
{
    uint64_t  id;
    uint64_t  size;
    uint64_t  vaddr;
} shm_ioctl_args_t;