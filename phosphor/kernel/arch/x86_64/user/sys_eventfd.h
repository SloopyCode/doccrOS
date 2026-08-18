/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_eventfd.h
 *
 */

#ifndef SYS_EVENTFD_H
#define SYS_EVENTFD_H

#include <types.h>
#include <kernel/arch/x86_64/idt/idt.h>

void sys_eventfd(cpu_state_t *state);
void sys_eventfd_open(cpu_state_t *state);

#endif