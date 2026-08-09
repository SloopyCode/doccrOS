/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: wait.h
 *
 */

#ifndef WAIT_H
#define WAIT_H

#include "thread.h"
#include <types.h>

typedef struct wait_queue
{
    thread_t *head;
    thread_t *tail;
} wait_queue_t;

void wait_queue_init(wait_queue_t *queue);
void wait_queue_block(wait_queue_t *queue);
void wait_queue_wake_one(wait_queue_t *queue);
void wait_queue_wake_all(wait_queue_t *queue);
void wait_queue_remove(thread_t *waiter);

#endif