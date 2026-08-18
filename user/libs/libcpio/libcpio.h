/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: libcpio.h
 *
 */

#pragma once

typedef struct
{
    int total_entries;
    int files_written;
    int dirs_created;
} cpio_extract_stats_t;

int cpio_extract_mem(
    const void *archive,
    unsigned long size,
    const char *dest_dir,
    cpio_extract_stats_t *out_stats
);

int cpio_extract_file(
    const char *cpio_path,
    const char *dest_dir,
    cpio_extract_stats_t *out_stats
);