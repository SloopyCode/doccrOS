/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: shm.c
 *
 */

#include "shm.h"
#include "../names.h"
#include <kernel/proc/process.h>
#include <kernel/mem/meminclude.h>
#include <kernel/screen/lib/log.h>

#define SHM_MAX_SEGMENTS 64
#define SHM_VADDR_BASE 0x0000700000000000ULL
#define SHM_VADDR_STRIDE 0x0000000010000000ULL  // 256 MiB per solt

typedef struct
{
    u64 id;
    u64 phys_base;
    u64 page_count;
    u64 size;
    i32 refcount;
    u8 used;
} shm_segment_t;

typedef struct
{
    proc_t *owner;
    u64 next_vaddr_slot;
} shm_handle_t;

static shm_segment_t segments[SHM_MAX_SEGMENTS];
static u64 next_id = 1;

static shm_segment_t *find_segment(u64 id)
{
    for (int i = 0; i < SHM_MAX_SEGMENTS; i++)
    {
    	if (
     		segments[i].used &&
       		segments[i].id == id
     	) return &segments[i];
    }
    return NULL;
}

static int shm_dev_init(void)
{
    memset(segments, 0, sizeof(segments));
    next_id = 1;
    log("[SHM]", "shared memory device ready\n");
    return 0;
}

static void *shm_open(const char *path)
{
    (void)path;
    proc_t *p = process_get_current();
    if (!p) return NULL;

    shm_handle_t *h = (shm_handle_t *) kmalloc(sizeof(shm_handle_t));
    if (!h) return NULL;

    h->owner            = p;
    h->next_vaddr_slot  = 0;
    return h;
}

static void shm_close(void *handle)
{
    kfree((u64 *)handle);
}

static i64 shm_ioctl(void *handle, u64 request, void *arg)
{
    shm_handle_t *h = (shm_handle_t *)handle;
    if (!h || !arg) return -1;

    shm_ioctl_args_t args;
    memcpy(&args, arg, sizeof(args));

    proc_t *p = h->owner;
    if (!p || !p->space) return -1;

    switch (request)
    {
        case SHM_IOCTL_ALLOC:
        {
            if (args.size == 0) return -1;

            u64 page_count = (args.size + PAGE_SIZE - 1) / PAGE_SIZE;
            u64 phys = physmem_alloc_to(page_count);
            if (!phys) return -1;

            int slot = -1;
            for (int i = 0; i < SHM_MAX_SEGMENTS; i++)
            {
            	if (!segments[i].used)
	            {
	            	slot = i; break;
	            }
            }

            if (slot < 0)
            {
                physmem_free_to(phys, page_count);
                return -1;
            }

            u64 vaddr = SHM_VADDR_BASE + (h->next_vaddr_slot++) * SHM_VADDR_STRIDE;

            u64 mapped = vmm_map_phys(
                p->space, vaddr, phys, page_count,
                VMM_REGION_USER | VMM_REGION_READ | VMM_REGION_WRITE
            );
            if (!mapped)
            {
                physmem_free_to(phys, page_count);
                return -1;
            }


            segments[slot] = (shm_segment_t){
                .id = next_id++, .phys_base = phys, .page_count = page_count,
                .size = args.size, .refcount = 1, .used = 1
            };

            args.id = segments[slot].id;
            args.size = page_count * PAGE_SIZE;
            args.vaddr = mapped;

            memcpy(arg, &args, sizeof(args));

            return 0;
        }

        case SHM_IOCTL_MAP:
        {
            shm_segment_t *seg = find_segment(args.id);
            if (!seg) return -1;

            u64 vaddr = SHM_VADDR_BASE + (h->next_vaddr_slot++) * SHM_VADDR_STRIDE;
            u64 mapped = vmm_map_phys(
                p->space, vaddr, seg->phys_base, seg->page_count,
                VMM_REGION_USER | VMM_REGION_READ | VMM_REGION_WRITE
            );
            if (!mapped) return -1;

            seg->refcount++;

            args.size = seg->size;
            args.vaddr = mapped;

            memcpy(arg, &args, sizeof(args));

            return 0;
        }

        case SHM_IOCTL_UNMAP:
        {
            shm_segment_t *seg = find_segment(args.id);
            if (!seg) return -1;

            vmm_unmap_phys(p->space, args.vaddr);

            seg->refcount--;
            if (seg->refcount <= 0)
            {
                physmem_free_to(seg->phys_base, seg->page_count);
                seg->used = 0;
            }
            return 0;
        }

        case SHM_IOCTL_GET_SIZE:
        {
            shm_segment_t *seg = find_segment(args.id);
            if (!seg) return -1;
            args.size = seg->size;
            memcpy(arg, &args, sizeof(args));
            return 0;
        }

        default: return -1;
    }
}

device_handler shm_device =
{
    .name    = SHM_NAME,
    .mount   = SHM_MOUNT,
    .version = SHM_VERSION,
    .init    = shm_dev_init,
    .fini    = NULL,
    .open    = shm_open,
    .close   = shm_close,
    .read    = NULL,
    .write   = NULL,
    .ioctl   = shm_ioctl,
};