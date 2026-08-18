/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys_process.h
 *
 */

#ifndef SYS_PROCESS_H
#define SYS_PROCESS_H

#include <types.h>
#include <kernel/arch/x86_64/idt/idt.h>

void sys_exit(cpu_state_t *state);
void sys_yield(cpu_state_t *state);
void sys_getpid(cpu_state_t *state);
void sys_dup2(cpu_state_t *state);
void sys_fork(cpu_state_t *state);
void sys_execve(cpu_state_t *state);
void sys_spawn(cpu_state_t *state);
void sys_waitpid(cpu_state_t *state);
void sys_getuid(cpu_state_t *state);
void sys_getgid(cpu_state_t *state);

#endif
