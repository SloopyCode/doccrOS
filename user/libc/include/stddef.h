/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: stddef.h
 *
 */

#pragma once

typedef unsigned long size_t;
typedef long ssize_t;
typedef long ptrdiff_t;

#ifndef NULL
#   define NULL ((void*)0)
#endif

#define offsetof(t, m) __builtin_offsetof(t, m)
