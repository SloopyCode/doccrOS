/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: cpu.h
 *
 */

#ifndef CPU_H
#define CPU_H

#include <types.h>

void cpu_detect(void);
const char* cpu_get_vendor(void);
const char* cpu_get_brand(void);

#endif
