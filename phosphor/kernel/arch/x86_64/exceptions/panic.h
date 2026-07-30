/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: panic.h
 *
 */

#ifndef PANIC_H
#define PANIC_H

#include <types.h>
#include"../idt/idt.h"

// Kernel panic
__attribute__((noreturn)) void panic(const char *message);
__attribute__((noreturn)) void panic_exception(cpu_state_t *state, const char *message);

#endif
