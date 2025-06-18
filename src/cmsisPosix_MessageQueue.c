// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

#define _GNU_SOURCE
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"
#include "cmsisPosix_Common.h"

// Structure to hold POSIX message queue and CMSIS attributes
typedef struct
{
    const char *name;           // Name of message queue
    uint32_t msg_count;         // Maximum number of messages in queue
    uint32_t msg_size;          // Size of each message
    uint32_t padded_msg_size;   // Padded size of each message
    sem_t avail;                // Number of message slots available
    sem_t used;                 // Number of message slots used
    pthread_mutex_t mutex;      // Mutex for buffer, priorities, head and tail
    uint8_t *buffer;            // Pre-allocated circular buffer for messages
    uint8_t *priorities;        // Array of message priorities
    uint32_t head;              // Index to head of circular buffer
    uint32_t tail;              // Index to tail of circular buffer
} cmsisPosix_messageQueueHandler_t;

// Finds the next index into a circular queue with a given capacity 
static uint32_t next_idx(uint32_t idx, uint32_t capacity)
{
    return (idx + 1) % capacity;
}

// Finds the previous index into a circular queue with a given capacity 
static uint32_t prev_idx(uint32_t idx, uint32_t capacity)
{
    return (idx + capacity - 1) % capacity;
}

osMessageQueueId_t osMessageQueueNew(uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr)
{
    if ((msg_count == 0) || (msg_size == 0))
    {
        return NULL;
    }

    // Calculate padded message size required to ensure each message starts at an aligned address
    uint32_t align_size = sizeof(max_align_t);
    uint32_t padded_msg_size = align_size * ((msg_size + align_size - 1) / align_size);

    cmsisPosix_messageQueueHandler_t *queue = malloc(sizeof(cmsisPosix_messageQueueHandler_t));
    if (queue == NULL)
    {
        return NULL;
    }

    if (sem_init(&queue->avail, 0, msg_count) != 0)
    {
        free(queue);
        return NULL;
    }

    if (sem_init(&queue->used, 0, 0) != 0)
    {
        sem_destroy(&queue->avail);
        free(queue);
        return NULL;
    }

    if (pthread_mutex_init(&queue->mutex, NULL) != 0)
    {
        sem_destroy(&queue->avail);
        sem_destroy(&queue->used);
        free(queue);
        return NULL;
    }

    queue->name = (attr ? attr->name : NULL);
    queue->msg_count = msg_count;
    queue->msg_size = msg_size;
    queue->padded_msg_size = padded_msg_size;
    queue->priorities = calloc(msg_count, sizeof(uint8_t));
    queue->head = 0;
    queue->tail = 0;

    // Instead of using cb_mem, a buffer of padded messages is allocated to ensure memory alignment for each message
    queue->buffer = malloc(msg_count * padded_msg_size);

    if ((queue->buffer == NULL) || (queue->priorities == NULL))
    {
        osMessageQueueDelete(queue);
        return NULL;
    }

    return (osMessageQueueId_t)queue;
}

const char *osMessageQueueGetName (osMessageQueueId_t mq_id)
{
    cmsisPosix_messageQueueHandler_t *queue = (cmsisPosix_messageQueueHandler_t *)mq_id;
    
    if (queue == NULL)
    {
        NULL;
    }

    return queue->name;
}

osStatus_t osMessageQueuePut(osMessageQueueId_t mq_id, const void* msg_ptr, uint8_t msg_prio, uint32_t timeout)
{
    cmsisPosix_messageQueueHandler_t *queue = (cmsisPosix_messageQueueHandler_t *)mq_id;
    
    if ((queue == NULL) || (msg_ptr == NULL))
    {
        return osErrorParameter;
    }

    // Try to reduce the number of available slots
    int posix_ret;
    if (timeout == 0)
    {
        // Do not wait
        posix_ret = sem_trywait(&queue->avail);
    } else if (timeout == osWaitForever)
    {
        // Block indefinitely
        posix_ret = sem_wait(&queue->avail);
    } else {
        struct timespec ts;
        cp_timeoutToTimespec(timeout, &ts);

        // Wait for the required duration
        posix_ret = sem_timedwait(&queue->avail, &ts);
    }

    if (posix_ret != 0)
    {
        if (errno == ETIMEDOUT)
        {
            return osErrorTimeout;
        } else {
            return osErrorResource;
        }
    }

    // Find message insertion position starting from tail of queue
    pthread_mutex_lock(&queue->mutex);
    uint32_t ins = queue->tail;
    while (ins != queue->head)
    {
        uint32_t prev = prev_idx(ins, queue->msg_count);
        if (msg_prio <= queue->priorities[prev])
        {
            // Previous message in queue has higher or same priority.  Insert message here
            break;
        } else {
            // Otherwise move previous message to make room for new message
            memcpy(queue->buffer + (ins * queue->padded_msg_size),
                    queue->buffer + (prev * queue->padded_msg_size), queue->msg_size);
            queue->priorities[ins] = queue->priorities[prev];

            // Continue searching from previous position
            ins = prev;
        }
    }

    // Copy new message and advance tail
    memcpy(queue->buffer + (ins * queue->padded_msg_size), msg_ptr, queue->msg_size);
    queue->priorities[ins] = msg_prio;
    queue->tail = next_idx(queue->tail, queue->msg_count);
    pthread_mutex_unlock(&queue->mutex);

    // Increase the number used slots
    sem_post(&queue->used);
    return osOK;
}

osStatus_t osMessageQueueGet(osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout)
{
    cmsisPosix_messageQueueHandler_t *queue = (cmsisPosix_messageQueueHandler_t *)mq_id;
    
    if ((queue == NULL) || (msg_ptr == NULL))
    {
        return osErrorParameter;
    }

    // Try to reduce the number of used slots
    int posix_ret;
    if (timeout == 0)
    {
        // Do not wait
        posix_ret = sem_trywait(&queue->used);
    } else if (timeout == osWaitForever)
    {
        // Block indefinitely
        posix_ret = sem_wait(&queue->used);
    } else {
        struct timespec ts;
        cp_timeoutToTimespec(timeout, &ts);

        // Wait for the required duration
        posix_ret = sem_timedwait(&queue->used, &ts);
    }

    if (posix_ret != 0)
    {
        if (errno == ETIMEDOUT)
        {
            return osErrorTimeout;
        } else {
            return osErrorResource;
        }
    }

    // Message available, copy from head of circular buffer and advance head
    pthread_mutex_lock(&queue->mutex);
    memcpy(msg_ptr, queue->buffer + (queue->head * queue->padded_msg_size), queue->msg_size);
    if (msg_prio)
    {
        *msg_prio = queue->priorities[queue->head];
    }
    queue->head = next_idx(queue->head, queue->msg_count);
    pthread_mutex_unlock(&queue->mutex);

    // Increase the number available slots
    sem_post(&queue->avail);
    return osOK;
}

uint32_t osMessageQueueGetCapacity(osMessageQueueId_t mq_id)
{
    cmsisPosix_messageQueueHandler_t *queue = (cmsisPosix_messageQueueHandler_t *)mq_id;
    
    if (queue == NULL)
    {
        return 0;
    }

    return queue->msg_count;
}

uint32_t osMessageQueueGetMsgSize(osMessageQueueId_t mq_id)
{
    cmsisPosix_messageQueueHandler_t *queue = (cmsisPosix_messageQueueHandler_t *)mq_id;
    
    if (queue == NULL)
    {
        return 0;
    }

    return queue->msg_size;
}

uint32_t osMessageQueueGetCount(osMessageQueueId_t mq_id)
{
    cmsisPosix_messageQueueHandler_t *queue = (cmsisPosix_messageQueueHandler_t *)mq_id;
    
    if (queue == NULL)
    {
        return 0;
    }

    int count = 0;
    if ((sem_getvalue(&queue->used, &count) != 0) || (count < 0))
    {
        count = 0;
    }
    return count;
}

uint32_t osMessageQueueGetSpace(osMessageQueueId_t mq_id)
{
    cmsisPosix_messageQueueHandler_t *queue = (cmsisPosix_messageQueueHandler_t *)mq_id;
    
    if (queue == NULL)
    {
        return 0;
    }

    int space = 0;
    if ((sem_getvalue(&queue->avail, &space) != 0) || (space < 0))
    {
        space = 0;
    }
    return space;
}

osStatus_t osMessageQueueDelete(osMessageQueueId_t mq_id)
{
    cmsisPosix_messageQueueHandler_t *queue = (cmsisPosix_messageQueueHandler_t *)mq_id;
    
    if (queue == NULL)
    {
        return osErrorParameter;
    }

    sem_destroy(&queue->avail);
    sem_destroy(&queue->used);
    pthread_mutex_destroy(&queue->mutex);
    free(queue->buffer);
    free(queue->priorities);
    free(queue);
    return osOK;
}