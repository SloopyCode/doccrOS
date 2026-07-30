/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_io.h
 *
 */

#ifndef SYS_IO_H
#define SYS_IO_H

#include <types.h>
#include <kernel/arch/x86_64/idt/idt.h>

void sys_read(cpu_state_t *state);
void sys_write(cpu_state_t *state);

#endif
