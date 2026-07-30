/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_power.h
 *
 */

#ifndef SYS_POWER_H
#define SYS_POWER_H

#include <types.h>
#include <kernel/arch/x86_64/idt/idt.h>

void sys_reboot(cpu_state_t *state);

#endif