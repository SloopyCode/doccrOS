/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys/eventfd.h
 *
 */

#pragma once

#define EFD_SEMAPHORE 0x00000001
#define EFD_NONBLOCK 0x00000800

// NOEXEC in future

long eventfd(unsigned int initial_value, int flags);