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
#if CP_SCHED != SCHED_OTHER
static uint32_t cp_threadMinPriority;
#endif

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
 * Function to ensure all threads are running on single CPU, no real parallel work.
 */
static void cp_threadUseSingleCpu()
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset); // Use CPU 0

    pthread_t thread = pthread_self();
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("pthread_setaffinity_np");
    }
}

/*
 * Wrapper for threads entry, converting the void* that returned by POSIX entry to void returned by cmsis entry.
 */
static void *cp_threadWrapper(void *arg)
{

    cmsisPosix_threadHandler_t *threadData = (cmsisPosix_threadHandler_t *)arg;

    threadData->thread = pthread_self(); // Storing here in case ran before the creation function ran

    pthread_setspecific(cp_threadDataKey, threadData);

    // Call the CMSIS-style thread function
    threadData->func(threadData->arg);

    return NULL;  // Unreachable
}

osStatus_t cp_threadInitSystem()
{
    if (pthread_key_create(&cp_threadDataKey, cp_threadDataCleanup) != 0)
    {
        return osError;
    }

    cp_threadUseSingleCpu();

#if CP_SCHED != SCHED_OTHER
    uint32_t threadMaxPriority;

    // In case of round robin/fifo, we really have priority between threads
    pthread_t curr_thread;
    struct sched_param curr_thread_param;

    cp_threadMinPriority = sched_get_priority_min(CP_SCHED);
    threadMaxPriority = sched_get_priority_max(CP_SCHED);

    if(threadMaxPriority - cp_threadMinPriority < osPriorityISR - osPriorityIdle)
    {
        return osError;
    }

    curr_thread = pthread_self();

    // This thread should be with higher priority thaqn all other, so no thread will be able to run before this thread exit in osKernelStart.
    curr_thread_param.sched_priority = osPriorityISR + cp_threadMinPriority;

    int ret = pthread_setschedparam(curr_thread, SCHED_FIFO, &curr_thread_param);
    assert(ret == 0);// If this fail, probably lack permisions to run
#endif

    return osOK;
}

osThreadId_t osThreadNew(osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
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

    if (pthread_attr_setschedpolicy(&pthread_attr, CP_SCHED) != 0)
    {
        pthread_attr_destroy(&pthread_attr);
        return NULL;
    }
    if(pthread_attr_setschedparam(&pthread_attr, &sched) != 0)
    {
        pthread_attr_destroy(&pthread_attr);
        return NULL;
    }
#endif

    pthread_attr_setinheritsched(&pthread_attr, PTHREAD_EXPLICIT_SCHED);

    threadData->func = func;
    threadData->arg = argument;

    int result = pthread_create(&thread, &pthread_attr, cp_threadWrapper, threadData);

    threadData->thread = thread; // Storing here in case ran before the wrapper function ran

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

osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority)
{
        if(NULL == thread_id)
    {
        return osErrorParameter;
    }

#if CP_SCHED != SCHED_OTHER
    cmsisPosix_threadHandler_t *thread_handler = (cmsisPosix_threadHandler_t *)thread_id;

    struct sched_param sched;
    sched.sched_priority = priority + cp_threadMinPriority;

    int ret = pthread_setschedparam(thread_handler->thread, CP_SCHED, &sched);
    assert(ret == 0);// If this fail, probably lack permisions to run
#else
    (void)priority;
#endif

    return osOK;
}

osThreadId_t osThreadGetId (void)
{
    cmsisPosix_threadHandler_t *thread_handler = (cmsisPosix_threadHandler_t *)pthread_getspecific(cp_threadDataKey);

    return (osThreadId_t)thread_handler;
}

osStatus_t osThreadYield (void)
{
    return (sched_yield() == 0) ? osOK : osError;
}

__NO_RETURN void osThreadExit(void)
{
    pthread_exit(NULL); // Terminates the calling thread
}
