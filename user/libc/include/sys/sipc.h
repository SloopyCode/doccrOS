/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: sys/sipc.h
 *
 */

#pragma once

#include <unistd.h>

// this is a temporary solution to a ipc for sulfurOS
long ipc_create(const char *name);
long ipc_open(const char *name);
long ipc_send(int fd, const void *buf, size_t count);
long ipc_recv(int fd, void *buf, size_t count, int flags);
long ipc_eventfd(int fd);