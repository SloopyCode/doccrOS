/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_power.c
 *
 */

#include "sys_power.h"
#include "ptr.h"
#include <kernel/arch/x86_64/poweroff.h>

void sys_reboot(cpu_state_t *state)
{
    int cmd = (int)state->rdi;

    if (cmd == POWEROFF_REBOOT) (POWEROFF_REBOOT);
    else cpu_poweroff(POWEROFF_SHUTDOWN);

    state->rax = (u64)-1;
}