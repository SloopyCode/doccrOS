/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_ipc.h
 *
 */

#ifndef SYS_IPC_H
#define SYS_IPC_H

#include <kernel/arch/x86_64/idt/idt.h>

void sys_ipc_create(cpu_state_t *state);
void sys_ipc_open(cpu_state_t *state);
void sys_ipc_send(cpu_state_t *state);
void sys_ipc_recv(cpu_state_t *state);
void sys_ipc_get_eventfd(cpu_state_t *state);

#endif
