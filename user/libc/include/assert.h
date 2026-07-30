/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: assert.h
 *
 */

#pragma once
#include <stdlib.h>
#define assert(x) ((x)?(void)0:exit(1))
