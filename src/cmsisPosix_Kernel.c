// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

#define _GNU_SOURCE
#include <time.h>
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"
#include "cmsisPosix_Common.h"
#include <pthread.h>

extern osStatus_t cp_threadInitSystem();

osStatus_t osKernelInitialize (void)
{
    return cp_threadInitSystem();
}

osStatus_t osKernelStart (void)
{
    pthread_exit(NULL);  // Terminates this thread
}

uint32_t osKernelGetTickCount (void)
{
    struct timespec ts;
    cp_timeoutToTimespec(0, &ts);

    // Get current time
    int64_t total_nanos = (ts.tv_sec * (int64_t)1000000000L) + ts.tv_nsec;
    return (uint32_t)(total_nanos / CP_CONFIG_NANO_IN_TICK);
}

uint32_t osKernelGetTickFreq (void)
{
        return (int64_t)1000000000L / CP_CONFIG_NANO_IN_TICK;
}