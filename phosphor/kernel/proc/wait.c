/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (c) 2026 sulfurLabs
 *
 * PROJECT: sulfurOS
 * FILE: wait.c
 *
 */

#include "wait.h"
#include "scheduler.h"
#include <kernel/arch/hal/irqflags.h>

void wait_queue_init(wait_queue_t *queue)
{
    queue->head = NULL;
    queue->tail = NULL;
}

static void wait_queue_enqueue(wait_queue_t *queue, thread_t *waiter)
{
    waiter->wait_next = NULL;
    waiter->wait_queue = queue;

    if (queue->tail) queue->tail->wait_next = waiter;
    else queue->head = waiter;

    queue->tail = waiter;
}

void wait_queue_block(wait_queue_t *queue)
{
    irq_state_t saved_state = irq_save();
    thread_t *current = thread_get_current();

    wait_queue_enqueue(queue, current);
    current->state = THREAD_BLOCKED;
    sched_yield();
    irq_restore(saved_state);
}

void wait_queue_wake_one(wait_queue_t *queue)
{
    irq_state_t saved_state = irq_save();
    thread_t *waiter = queue->head;

    if (waiter)
    {
        queue->head = waiter->wait_next;
        if (!queue->head) queue->tail = NULL;
        waiter->wait_next = NULL;
        waiter->wait_queue = NULL;

        sched_add(waiter);
    }

    irq_restore(saved_state);
}

void wait_queue_wake_all(wait_queue_t *queue)
{
    irq_state_t saved_state = irq_save();
    thread_t *waiter = queue->head;

    while (waiter)
    {
        thread_t *next = waiter->wait_next;

        waiter->wait_next = NULL;
        waiter->wait_queue = NULL;
        sched_add(waiter);

        waiter = next;
    }

    queue->head = NULL;
    queue->tail = NULL;

    irq_restore(saved_state);
}

void wait_queue_remove(thread_t *waiter)
{
    if (!waiter || !waiter->wait_queue) return; // wasnt waiting on anything

    irq_state_t saved_state = irq_save();
    wait_queue_t *queue = waiter->wait_queue;

    if (queue)
    {
        thread_t *cursor = queue->head;
        thread_t *previous = NULL;

        while (cursor)
        {
            if (cursor == waiter)
            {
                if (previous) previous->wait_next = cursor->wait_next;
                else queue->head = cursor->wait_next;

                if (queue->tail == cursor) queue->tail = previous;

                break;
            }

            previous = cursor;
            cursor = cursor->wait_next;
        }
    }

    waiter->wait_next = NULL;
    waiter->wait_queue = NULL;
    irq_restore(saved_state);
}