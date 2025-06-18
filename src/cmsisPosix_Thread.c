// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"

static pthread_key_t cp_threadDataKey;
static uint32_t cp_threadMinPriority;

/*
 * Struct for thread wrapper input
 */
typedef struct
{
    osThreadFunc_t func;
    void *arg;
    const char *name;
    pthread_t thread;
} cmsisPosix_threadHandler_t;

// Cleanup function to free allocated memory
static void cp_threadDataCleanup(void *ptr)
{
    free(ptr);
}

/*
 * Wrapper for threads entry, converting the void* that returned by POSIX entry to void returned by cmsis entry.
 */
static void *cp_threadWrapper(void *arg)
{
    cmsisPosix_threadHandler_t *threadData = (cmsisPosix_threadHandler_t *)arg;

    pthread_setspecific(cp_threadDataKey, threadData);

    // Call the CMSIS-style thread function
    threadData->func(threadData->arg);

    return NULL;  // Unreachable
}

osStatus_t cp_threadInitSystem()
{
    uint32_t threadMaxPriority;
    if (pthread_key_create(&cp_threadDataKey, cp_threadDataCleanup) != 0)
    {
        return osError;
    }

    cp_threadMinPriority = sched_get_priority_min(CP_SCHED);
    threadMaxPriority = sched_get_priority_max(CP_SCHED);

    if(threadMaxPriority - cp_threadMinPriority < osPriorityISR - osPriorityIdle)
    {
        return osError;
    }

    return osOK;
}

osThreadId_t osThreadNew(osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
    (void)attr;
    pthread_t thread;
    pthread_attr_t pthread_attr;
    cmsisPosix_threadHandler_t *threadData;

    if (NULL == func)
    {
      return NULL;
    }

    // Allocate wrapper to pass function + argument
    threadData = malloc(sizeof(cmsisPosix_threadHandler_t));
    if (NULL == threadData)
    {
        return NULL;
    }

    pthread_attr_init(&pthread_attr);

    if(attr)
    {
        threadData->name = attr->name;
    }
    else
    {
        threadData->name = NULL;
    }

#if CP_SCHED != SCHED_OTHER
    struct sched_param sched;
    if(attr && attr->priority > 0)
    {
        sched.sched_priority = attr->priority + cp_threadMinPriority;
    }
    else
    {
        sched.sched_priority = osPriorityNormal + cp_threadMinPriority;
    }

    if (pthread_attr_setschedpolicy(&pthread_attr, CP_SCHED) != 0 ||
        pthread_attr_setschedparam(&pthread_attr, &sched) != 0)
    {
        printf("Warning: Unable to set thread scheduling. Try running with sudo.\n");
    }
#endif

    pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);

    threadData->func = func;
    threadData->arg = argument;
    threadData->thread = thread;

    int result = pthread_create(&thread, &pthread_attr, cp_threadWrapper, threadData);
    pthread_attr_destroy(&pthread_attr);

    if (result != 0)
    {
        free(threadData);
        return NULL;
    }

    // Detach by default (CMSIS doesn't require joinable threads)
    pthread_detach(thread);

    return (osThreadId_t)threadData;  // Cast for compatibility
}

const char *osThreadGetName (osThreadId_t thread_id)
{
    if(NULL == thread_id)
    {
        return NULL;
    }

    return ((cmsisPosix_threadHandler_t *)thread_id)->name;
}

osThreadId_t osThreadGetId (void)
{
    cmsisPosix_threadHandler_t *thread_handler = (cmsisPosix_threadHandler_t *)pthread_getspecific(cp_threadDataKey);

    return (osThreadId_t)thread_handler;
}

__NO_RETURN void osThreadExit(void)
{
    pthread_exit(NULL); // Terminates the calling thread
}
