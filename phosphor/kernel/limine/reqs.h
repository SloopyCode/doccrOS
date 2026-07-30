/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: reqs.h
 *
 */

#ifndef REQS_H
#define REQS_H

#include <limine/limine.h>

// Limine requests
extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_module_request module_request; // how the initrd sneaks in

#endif
