/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_uname.c
 *
 */

#include "sys_uname.h"
#include "ptr.h"
#include <kernel/screen/lib/string.h>
#include <kernel/mem/mem.h>
#include <build.h>

#define K_UTSNAME_LENGTH 65

typedef struct
{
    char sysname[K_UTSNAME_LENGTH];
    char nodename[K_UTSNAME_LENGTH];
    char release[K_UTSNAME_LENGTH];
    char version[K_UTSNAME_LENGTH];
    char machine[K_UTSNAME_LENGTH];
    char domainname[K_UTSNAME_LENGTH];
} k_utsname_t;

void sys_uname(cpu_state_t *state)
{
    k_utsname_t *user_buf = (k_utsname_t *)state->rdi;

    if (!user_ptr_ok((u64)user_buf))
    {
        state->rax = (u64)-1;
        return;
    }

    k_utsname_t out;
    memset(&out, 0, sizeof(out));

    str_copy(out.sysname, "sulfurOS");
    str_copy(out.nodename, "s4");
    str_copy(out.release, "0.7.0");
    str_copy(out.version, ___SULF_VER " " ___SULF_BUILD);
    str_copy(out.machine, "x86_64");
    str_copy(out.domainname, "(none)");

    memcpy(user_buf, &out, sizeof(out));

    state->rax = 0;
}