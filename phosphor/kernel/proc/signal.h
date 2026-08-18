/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: signal.h
 *
 */

#pragma once

#include <types.h>
#include <kernel/arch/x86_64/idt/idt.h>
#include "process.h"

#define SIGINT 2
#define SIGTERM 15

void signal_proc_init(proc_t *p);
void signal_on_fork(proc_t *child, proc_t *parent);
void signal_on_exec(proc_t *p);

int sys_kill_impl(u64 pid, int sig);
u64 sys_signal_impl(int sig, u64 handler, int *out_ok);
void sys_sigreturn_impl(cpu_state_t *state);
void sys_set_sigtramp_impl(u64 addr);
void signal_check_and_deliver(cpu_state_t *state);
