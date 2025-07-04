// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

#define _GNU_SOURCE
#include <errno.h>

#include <stdint.h>
#include <time.h>
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"
#include "cmsisPosix_Common.h"

osStatus_t osDelay(uint32_t ticks)
{
    struct timespec ts;
    cp_timeoutToTimespec(ticks, &ts);

    // Sleep until it is time
    while (1)
    {
        int posix_ret =  clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL);
        if (posix_ret == 0)
        {
            return osOK;
        } else if (posix_ret != EINTR)
        {
            return osError;
        }
    }
}

osStatus_t osDelayUntil (uint32_t ticks)
{
    // Handle tick counter wrapping by converting to relative delay duration
    uint32_t delta = ticks - osKernelGetTickCount();
    return osDelay(delta);
}