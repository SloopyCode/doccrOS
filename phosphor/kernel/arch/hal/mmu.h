/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: mmu.h
 *
 */

#pragma once
#include <types.h>

void arch_mmu_activate(u64 table_phys);
