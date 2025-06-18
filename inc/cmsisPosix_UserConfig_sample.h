// Copyright (c) 2025 Arye Gross
// SPDX-License-Identifier: Apache-2.0

/*
 * CP_CONFIG_SCHEDULING
 * This define control the scheduling algorithm used for CMSIS threads.
 *
 * CP_CONFIG_SCHEDULING_NICE:
 *   * Uses POSIX nice-based scheduling.
 *   * priority are totally ignored.
 *   * threads may start running before osKernelStart is called.
 *   
 *   (Note: This behavior differs from typical RTOS preemption. Use just if you can't grant the process the needed privilages)
 *
 * CP_CONFIG_SCHEDULING_ROUND_ROBIN:
 *   Uses POSIX SCHED_RR. The highest-priority thread always runs. If multiple threads have the same priority, they share CPU time.
 *   (Similar to FreeRTOS configUSE_TIME_SLICING=1 and ThreadX with finite time_slice.)
 *   ! Requires running the application with elevated privileges (e.g., sudo).
 *
 * CP_CONFIG_SCHEDULING_FIFO:
 *   Uses POSIX SCHED_FIFO. The highest-priority thread always runs. Among equal-priority threads, the currently running one continues until it blocks.
 *   (Similar to FreeRTOS configUSE_TIME_SLICING=0 and ThreadX with infinite time_slice.)
 *   ! Requires running the application with elevated privileges (e.g., sudo).
 */
//#define CP_CONFIG_SCHEDULING CP_CONFIG_SCHEDULING_NICE


/*
 * CP_CONFIG_NANO_IN_TICK
 * This define mocks kernel tick, used for CMSIS_RTOS_TimeOutValue and such.
 * The value shouldn't be bigger than 1 second
 */
// #define CP_CONFIG_NANO_IN_TICK 1000000L // 1 mSec
