/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_uname.h
 *
 */

#ifndef SYS_UNAME_H
#define SYS_UNAME_H

#include <types.h>
#include <kernel/arch/x86_64/idt/idt.h>

void sys_uname(cpu_state_t *state);

#endif