// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

/*
 * Forward declaration of common functions internal to CMSIS-POSIX.
 */

#ifndef CMSIS_POSIX_COMMON_H_
#define CMSIS_POSIX_COMMON_H_

#include <time.h>

#ifdef  __cplusplus
extern "C" {
#endif

void cp_timeoutToTimespec(uint32_t timeout, struct timespec *ts);

#ifdef  __cplusplus
}
#endif

#endif // CMSIS_POSIX_COMMON_H_
