/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: mmu.c
 *
 */

#include <kernel/arch/hal/mmu.h>

void arch_mmu_activate(u64 table_phys)
{
    __asm__ volatile("mov %0, %%cr3" : : "r"(table_phys) : "memory");
}
