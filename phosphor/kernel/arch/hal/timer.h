/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: timer.h
 *
 */

#pragma once
#include <types.h>
void     timer_init(u32 frequency);
u64      timer_get_ticks(void);
void     timer_wait(u32 ticks);
void timer_set_boot_time(void);
