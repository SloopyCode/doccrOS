/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_process.c
 *
 */

#include "sys_process.h"
#include <kernel/proc/thread.h>
#include <kernel/proc/process.h>
#include <kernel/proc/scheduler.h>
#include <kernel/fs/vfs/vfs.h>
#include <kernel/packages/elf/elf.h>
#include <kernel/communication/serial.h>
#include <kernel/mem/phys/physmem.h>
#include <kernel/mem/kheap/kheap.h>

static int user_ptr_ok(u64 ptr)
{
    return ptr != 0 && ptr <= 0x00007FFFFFFFFFFFULL;
}

void sys_exit(cpu_state_t *state)
{
    (void)state;
    thread_exit();
}

void sys_yield(cpu_state_t *state)
{
    sched_yield();
    state->rax = 0;
}

void sys_getpid(cpu_state_t *state)
{
    proc_t *p = process_get_current();
    state->rax = p ? p->pid : (u64)-1;
}

void sys_fork(cpu_state_t *state)
{
    printf("[SYS_FORK] before: physmem_free=%llu kheap_free=%llu\n",
           physmem_free_get(), kheap_get_free_size());

    proc_t *child = process_fork(state);

    printf("[SYS_FORK] after: physmem_free=%llu kheap_free=%llu result=%s\n",
           physmem_free_get(), kheap_get_free_size(), child ? "OK" : "FAILED");

    state->rax = child ? child->pid : (u64)-1;
}

void sys_execve(cpu_state_t *state)
{
    const char *path =  (const char *)state->rdi;

    if (!user_ptr_ok((u64)path))
    {
        state->rax = (u64)-1;
        return;
    }

    char path_buf[VFS_MAX_PATH];
    int i = 0;
    while (path[i]  && i < VFS_MAX_PATH - 1)
    {
        path_buf[i] = path[i];
        i++;
    }
    path_buf[i] = '\0';

    vfs_node_t *node = vfs_find(path_buf);
    if (!node || node->type != VFS_FILE || !node->data || node->size == 0)
    {
        state->rax = (u64)-1;
        return;
    }

    proc_t *p = process_get_current();
    if (!p)
    {
        state->rax = (u64)-1;
        return;
    }

    char name_buf[64];
    {
        const char *base   = path_buf;
        for (const char *s = path_buf; *s; s++) if (*s == '/') base = s + 1;

        int j = 0;
        while (base[j] && j < 63)
        {
        	name_buf[j] = base[j];
         	j++;
        }
        name_buf[j] = '\0';
    }

    if (elf_exec_replace(p, state, node->data, node->size, name_buf) != 0)
    {
        printf("[SYS_EXECVE] exec of '%s' failed, killing pid=%llu\n", path_buf, p->pid);

        thread_t *self = thread_get_current();
        if (self)
        {
            self->state = THREAD_DEAD;
            if (p->alive_count > 0) p->alive_count--;
        }
        process_exit(p, 1);
        sched_yield();

        __asm__ volatile("sti");
        for (;;) __asm__ volatile("hlt");
    }
}

void sys_waitpid(cpu_state_t *state)
{
    i64 target_pid = (i64)state->rdi;
    int *wstatus_ptr = (int *)state->rsi;

    proc_t *caller    = process_get_current();
    if (!caller)
    {
        state->rax    = (u64)-1;
        return;
    }

    int exit_code    = 0;
    int result       = process_waitpid(caller, target_pid, &exit_code);

    if (result != 0)
    {
        state->rax   = (u64)-1; // no dead kids found
        return;
    }


    if (
    	wstatus_ptr &&
     	(u64)wstatus_ptr <= 0x00007FFFFFFFFFFFULL
    )*wstatus_ptr  = (exit_code & 0xFF) << 8;

    state->rax     = (u64)target_pid;
}

//always 0 for now cuz were jst root
void sys_getuid(cpu_state_t *state)
{
    proc_t *p = process_get_current();
    state->rax = p ? (u64)p->uid : 0; // 0 = root, this is fine trust me
}

void sys_getgid(cpu_state_t *state)
{
    proc_t *p = process_get_current();
    state->rax = p ? (u64)p->gid : 0;
}
