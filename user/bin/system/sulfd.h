/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sulfd.h
 *
 */

#pragma once

// start the desktop by default
#define DESKTOP "/system/desktop/desktop.elf"

#define __INIT_SYSTEM_VER 1
#define __SULFD "sulfd"
#define __SULFD_BRACKETS "[" __SULFD "]"

#define SEPERATOR "$"

#define SULFHANDLERS { \
    "sulfur",    /*bs process, fbcon*/   \
    "kernel"     /*bs process */         \
    ,SEPERATOR,                          \
    "kernel",    /*kernel process*/      \
    "__rt"       /*kernel process*/ \
};