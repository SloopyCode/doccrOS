/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: irqflags.h
 *
 */

#pragma once
#include <types.h>

typedef u64 irq_state_t;

irq_state_t irq_save(void);

void irq_restore(irq_state_t state);