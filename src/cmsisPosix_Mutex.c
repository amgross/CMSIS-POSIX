#define _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"

// Forward declarations
void cp_timeoutToTimespec(uint32_t timeout, struct timespec *ts);

// TODO add comment why we prefer not to use cb mem (So we alwais need to free and not depends on what we got)

// Structure to hold POSIX mutex and CMSIS attributes
typedef struct {
    pthread_mutex_t mutex;
    const char *name;
} mutexMockHandler_t;

osMutexId_t osMutexNew(const osMutexAttr_t *attr)
{
    // Allocate memory for our mutex wrapper
    mutexMockHandler_t *mutex;

    // Initialize the mutex attributes
    pthread_mutexattr_t mutex_attr;
    if (pthread_mutexattr_init(&mutex_attr) != 0) {
        return NULL;
    }

    mutex = (mutexMockHandler_t *)malloc(sizeof(mutexMockHandler_t));
    if (mutex == NULL) {
        return NULL;  // Memory allocation failed
    }


    // Set up mutex according to CMSIS attributes if provided
    if (attr != NULL) {
        // Set name provided (If NULL then NULL)
        mutex->name = attr->name;
    } else {
        // Default attributes
        mutex->name = NULL;
    }

    // Initialize the mutex with attributes
    if (pthread_mutex_init(&mutex->mutex, &mutex_attr) != 0) {
        pthread_mutexattr_destroy(&mutex_attr);
        free(mutex);
        return NULL;
    }

    // Clean up the attributes
    pthread_mutexattr_destroy(&mutex_attr);

    return (osMutexId_t)mutex;
}

osStatus_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout)
{
    mutexMockHandler_t *mutex = (mutexMockHandler_t *)mutex_id;
    int posix_ret;
    
    if (mutex == NULL)
    {
        return osErrorParameter;
    }
    
    if (timeout == 0)
    {
        // Try to lock without waiting
        if (pthread_mutex_trylock(&mutex->mutex) == 0)
        {
            return osOK;
        }
        else
        {
            return osErrorResource;
        }
    }
    else if (timeout == osWaitForever)
    {
        // Block indefinitely
        if (pthread_mutex_lock(&mutex->mutex) == 0)
        {
            return osOK;
        }
        else
        {
            return osErrorResource;
        }
    }
    else
    {
        struct timespec ts;
        cp_timeoutToTimespec(timeout, &ts);

        posix_ret = pthread_mutex_timedlock(&mutex->mutex, &ts);
        if(0 == posix_ret)
        {
            return osOK;
        }
        else if(ETIMEDOUT == posix_ret)
        {
            return osErrorTimeout;
        }
        else
        {
            return osErrorResource;
        }
    }
}

osStatus_t osMutexRelease(osMutexId_t mutex_id)
{
    mutexMockHandler_t *mutex = (mutexMockHandler_t *)mutex_id;
    
    if (mutex == NULL) {
        return osErrorParameter;
    }
    
    if (pthread_mutex_unlock(&mutex->mutex) == 0)
    {
        return osOK;
    }
    else
    {
        return osErrorResource;
    }
}

osStatus_t osMutexDelete(osMutexId_t mutex_id) {
    mutexMockHandler_t *mutex = (mutexMockHandler_t *)mutex_id;
    
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
    mutexMockHandler_t *mutex = (mutexMockHandler_t *)mutex_id;
    
    if (mutex == NULL)
    {
        return NULL;
    }
    
    return mutex->name;
}