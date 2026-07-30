/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: logo.h
 *
 */

#ifndef LOGO_H
#define LOGO_H

#include <types.h>

extern const u32 logo_width;
extern const u32 logo_height;
extern const u8 logo[];

void draw_logo(void);

#define LOGO_SCALE 5

extern const char *small_logo_text;
//extern const char *big_logo;

#endif
#pragma once
