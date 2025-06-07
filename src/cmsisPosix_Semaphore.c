#define _GNU_SOURCE
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"

// Forward declarations
void cp_timeoutToTimespec(uint32_t timeout, struct timespec *ts);

// Structure to hold POSIX semaphore and CMSIS attributes
typedef struct
{
    const char *name;   // Name of semaphore
    sem_t sem;          // POSIX semaphore
} cp_semaphoreData_t;

osSemaphoreId_t osSemaphoreNew(uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr)
{
    if (max_count == 0) {
        return NULL;
    }

    cp_semaphoreData_t *semaphore = malloc(sizeof(cp_semaphoreData_t));
    if (semaphore == NULL) {
        return NULL;
    }

    if (sem_init(&semaphore->sem, 0, max_count) != 0) {
        free(semaphore);
        return NULL;
    }

    semaphore->name = (attr ? attr->name : NULL);

    // Lock semaphore until its value is less than or equals initial_count
    while (osSemaphoreGetCount(semaphore) > initial_count) {
    osSemaphoreAcquire(semaphore, 0);
    }

    return (osSemaphoreId_t)semaphore;
}

const char *osSemaphoreGetName (osSemaphoreId_t semaphore_id)
{
    cp_semaphoreData_t *semaphore = (cp_semaphoreData_t *)semaphore_id;
    
    if (semaphore == NULL) {
        NULL;
    }

    return semaphore->name;
}

osStatus_t osSemaphoreAcquire(osSemaphoreId_t semaphore_id, uint32_t timeout)
{
    cp_semaphoreData_t *semaphore = (cp_semaphoreData_t *)semaphore_id;
    
    if (semaphore == NULL) {
        return osErrorParameter;
    }

    // Lock the semaphore
    int posix_ret;
    if (timeout == 0) {
        // Try to lock without waiting
        posix_ret = sem_trywait(&semaphore->sem);
    } else if (timeout == osWaitForever) {
        // Block indefinitely
        posix_ret = sem_wait(&semaphore->sem);
    } else {
        struct timespec ts;
        cp_timeoutToTimespec(timeout, &ts);

        // Wait for the required duration
        posix_ret = sem_timedwait(&semaphore->sem, &ts);
    }

    if (posix_ret == 0) {
        return osOK;
    } else if (errno == ETIMEDOUT) {
        return osErrorTimeout;
    } else {
        return osErrorResource;
    }
}

osStatus_t osSemaphoreRelease(osSemaphoreId_t semaphore_id)
{
    cp_semaphoreData_t *semaphore = (cp_semaphoreData_t *)semaphore_id;
    
    if (semaphore == NULL) {
        return osErrorParameter;
    }

    sem_post(&semaphore->sem);
    return osOK;
}

uint32_t osSemaphoreGetCount(osSemaphoreId_t semaphore_id)
{
    cp_semaphoreData_t *semaphore = (cp_semaphoreData_t *)semaphore_id;
    
    if (semaphore == NULL) {
        return 0;
    }

    int count = 0;
    sem_getvalue(&semaphore->sem, &count);
    return count;
}

osStatus_t osSemaphoreDelete(osSemaphoreId_t semaphore_id)
{
    cp_semaphoreData_t *semaphore = (cp_semaphoreData_t *)semaphore_id;
    
    if (semaphore == NULL) {
        return osErrorParameter;
    }

    sem_destroy(&semaphore->sem);
    free(semaphore);
    return osOK;
}