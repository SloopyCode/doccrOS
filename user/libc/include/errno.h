/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: errno.h
 *
 */

#pragma once
int *__errno_location(void);
#define errno (*__errno_location())
#define EIO 5
#define EINTR 4
#define EISDIR 21
#define ENOENT 2
#define ENOMEM 12
