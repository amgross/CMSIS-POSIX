// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

#define _GNU_SOURCE
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"
#include "cmsisPosix_Common.h"

// Structure to hold semaphore and CMSIS attributes
typedef struct
{
    const char *name;   // Name of semaphore
    sem_t avail;        // Number of tokens available in semaphore
    sem_t used;         // Number of tokens used in semaphore
} cmsisPosix_semaphoreHandler_t;

osSemaphoreId_t osSemaphoreNew(uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr)
{
    if ((max_count == 0) || (max_count < initial_count))
    {
        return NULL;
    }

    cmsisPosix_semaphoreHandler_t *semaphore = malloc(sizeof(cmsisPosix_semaphoreHandler_t));
    if (semaphore == NULL)
    {
        return NULL;
    }

    if (sem_init(&semaphore->avail, 0, initial_count) != 0)
    {
        free(semaphore);
        return NULL;
    }

    if (sem_init(&semaphore->used, 0, max_count - initial_count) != 0)
    {
        sem_destroy(&semaphore->avail);
        free(semaphore);
        return NULL;
    }

    semaphore->name = (attr ? attr->name : NULL);

    return (osSemaphoreId_t)semaphore;
}

const char *osSemaphoreGetName (osSemaphoreId_t semaphore_id)
{
    cmsisPosix_semaphoreHandler_t *semaphore = (cmsisPosix_semaphoreHandler_t *)semaphore_id;
    
    if (semaphore == NULL)
    {
        NULL;
    }

    return semaphore->name;
}

osStatus_t osSemaphoreAcquire(osSemaphoreId_t semaphore_id, uint32_t timeout)
{
    cmsisPosix_semaphoreHandler_t *semaphore = (cmsisPosix_semaphoreHandler_t *)semaphore_id;
    
    if (semaphore == NULL)
    {
        return osErrorParameter;
    }

    // Try to reduce the number of available tokens
    int posix_ret;
    if (timeout == 0)
    {
        // Do not wait
        posix_ret = sem_trywait(&semaphore->avail);
    } else if (timeout == osWaitForever)
    {
        // Block indefinitely
        posix_ret = sem_wait(&semaphore->avail);
    } else {
        struct timespec ts;
        cp_timeoutToTimespec(timeout, &ts);

        // Wait for the required duration
        posix_ret = sem_timedwait(&semaphore->avail, &ts);
    }

    if (posix_ret == 0)
    {
        // Increment the number of used tokens
        sem_post(&semaphore->used);
        return osOK;
    } else if (errno == ETIMEDOUT)
    {
        return osErrorTimeout;
    } else {
        return osErrorResource;
    }
}

osStatus_t osSemaphoreRelease(osSemaphoreId_t semaphore_id)
{
    cmsisPosix_semaphoreHandler_t *semaphore = (cmsisPosix_semaphoreHandler_t *)semaphore_id;
    
    if (semaphore == NULL)
    {
        return osErrorParameter;
    }

    // Try to reduce the number of used tokens
    if (sem_trywait(&semaphore->used) == 0)
    {
        // Increment the number of available tokens
        sem_post(&semaphore->avail);
        return osOK;
    } else {
        return osErrorResource;
    }
}

uint32_t osSemaphoreGetCount(osSemaphoreId_t semaphore_id)
{
    cmsisPosix_semaphoreHandler_t *semaphore = (cmsisPosix_semaphoreHandler_t *)semaphore_id;
    
    if (semaphore == NULL)
    {
        return 0;
    }

    int count = 0;
    if ((sem_getvalue(&semaphore->avail, &count) < 0) || (count < 0))
    {
        count = 0;
    }
    return count;
}

osStatus_t osSemaphoreDelete(osSemaphoreId_t semaphore_id)
{
    cmsisPosix_semaphoreHandler_t *semaphore = (cmsisPosix_semaphoreHandler_t *)semaphore_id;
    
    if (semaphore == NULL)
    {
        return osErrorParameter;
    }

    sem_destroy(&semaphore->avail);
    sem_destroy(&semaphore->used);
    free(semaphore);
    return osOK;
}