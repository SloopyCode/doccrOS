/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys/stat.h
 *
 */

#pragma once
#include <sys/types.h>
struct stat { mode_t st_mode; off_t st_size; };
int mkdir(const char *, mode_t);
