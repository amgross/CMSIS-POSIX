// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

#define _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"
#include "cmsisPosix_Common.h"

// TODO add comment why we prefer not to use cb mem (So we alwais need to free and not depends on what we got)

// Structure to hold POSIX mutex and CMSIS attributes
typedef struct {
    pthread_mutex_t mutex;
    const char *name;
    osThreadId_t *owner;
} cmsisPosix_mutexHandler_t;

osMutexId_t osMutexNew(const osMutexAttr_t *attr)
{
    // Allocate memory for our mutex wrapper
    cmsisPosix_mutexHandler_t *mutex = NULL;

    // Initialize the mutex attributes
    pthread_mutexattr_t mutex_attr;
    if (pthread_mutexattr_init(&mutex_attr) != 0)
    {
        return NULL;
    }

    mutex = (cmsisPosix_mutexHandler_t *)malloc(sizeof(cmsisPosix_mutexHandler_t));
    if (mutex == NULL)
    {
        goto error_exit;
    }


    // Set up mutex according to CMSIS attributes if provided
    if (attr != NULL)
    {
        // Set name provided (If NULL then NULL)
        mutex->name = attr->name;

        //TODO deal with all other attributes
    }
    else
    {
        // Default attributes
        mutex->name = NULL;
    }
    
    // Initialize the mutex with attributes
    if (pthread_mutex_init(&mutex->mutex, &mutex_attr) != 0)
    {
        goto error_exit;
    }

    // Clean up the attributes
    pthread_mutexattr_destroy(&mutex_attr);

    return (osMutexId_t)mutex;

error_exit:
    pthread_mutexattr_destroy(&mutex_attr);
    free(mutex);
    return NULL;
}

osStatus_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout)
{
    cmsisPosix_mutexHandler_t *mutex = (cmsisPosix_mutexHandler_t *)mutex_id;
    int posix_ret;
    osThreadId_t curr_thread;
    
    if (mutex == NULL)
    {
        return osErrorParameter;
    }

    curr_thread = osThreadGetId();
    if(curr_thread == NULL)
    {
        return osErrorResource;
    }

    if (timeout == 0)
    {
        // Try to lock without waiting
        if (pthread_mutex_trylock(&mutex->mutex) != 0)
        {
            return osErrorResource;
        }
    }
    else if (timeout == osWaitForever)
    {
        // Block indefinitely
        if (pthread_mutex_lock(&mutex->mutex) != 0)
        {
            return osErrorResource;
        }
    }
    else
    {
        struct timespec ts;
        cp_timeoutToTimespec(timeout, &ts);

        posix_ret = pthread_mutex_timedlock(&mutex->mutex, &ts);
        if(0 != posix_ret)
        {
            if(ETIMEDOUT == posix_ret)
            {
                return osErrorTimeout;
            }
            else
            {
                return osErrorResource;
            }
        }
    }

    mutex->owner = curr_thread;

    return osOK;
}

osStatus_t osMutexRelease(osMutexId_t mutex_id)
{
    cmsisPosix_mutexHandler_t *mutex = (cmsisPosix_mutexHandler_t *)mutex_id;
    
    if (mutex == NULL)
    {
        return osErrorParameter;
    }

    if (pthread_mutex_unlock(&mutex->mutex) == 0)
    {
        // No need to ensure current thread is the owner before releasing, this is handled by POSIX
        mutex->owner = NULL;
        return osOK;
    }
    else
    {
        return osErrorResource;
    }
}

osThreadId_t osMutexGetOwner (osMutexId_t mutex_id)
{
    cmsisPosix_mutexHandler_t *mutex = (cmsisPosix_mutexHandler_t *)mutex_id;

    if(mutex == NULL)
    {
        return NULL;
    }

    return mutex->owner;
}

osStatus_t osMutexDelete(osMutexId_t mutex_id)
{
    cmsisPosix_mutexHandler_t *mutex = (cmsisPosix_mutexHandler_t *)mutex_id;
    
    if (mutex == NULL)
    {
        return osErrorParameter;
    }
    
    if (pthread_mutex_destroy(&mutex->mutex) == 0)
    {
        free(mutex);
        return osOK;
    }
    else
    {
        return osErrorResource;
    }
}

const char *osMutexGetName(osMutexId_t mutex_id)
{
    cmsisPosix_mutexHandler_t *mutex = (cmsisPosix_mutexHandler_t *)mutex_id;
    
    if (mutex == NULL)
    {
        return NULL;
    }
    
    return mutex->name;
}