
#include <assert.h>

/*
 * This define control the scheduling algorithm used for CMSIS threads.
 *
 * CP_CONFIG_SCHEDULING_NICE:
 *   Uses POSIX nice-based scheduling. Lower-priority threads may still run while higher-priority threads are active.
 *   (Note: This behavior differs from typical RTOS preemption.)
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
#define CP_CONFIG_SCHEDULING_NICE 0
#define  CP_CONFIG_SCHEDULING_ROUND_ROBIN 1
#define  CP_CONFIG_SCHEDULING_FIFO 2

#ifndef CP_CONFIG_SCHEDULING
#define CP_CONFIG_SCHEDULING CP_CONFIG_SCHEDULING_NICE
#endif

#if CP_CONFIG_SCHEDULING == CP_CONFIG_SCHEDULING_NICE
#define CP_SCHED SCHED_OTHER
#elif CP_CONFIG_SCHEDULING == CP_CONFIG_SCHEDULING_ROUND_ROBIN
#define CP_SCHED SCHED_RR
#elif CP_CONFIG_SCHEDULING == CP_CONFIG_SCHEDULING_FIFO
#define CP_SCHED SCHED_FIFO
#else
#error CP_CONFIG_SCHEDULING not defined (or defined with bad value)
#endif

/*
 * This define mocks kernel tick, used for CMSIS_RTOS_TimeOutValue and such
 */
#ifndef CP_CONFIG_NANO_IN_TICK
#define CP_CONFIG_NANO_IN_TICK 1000000L // 1 mSec
#endif

static_assert(CP_CONFIG_NANO_IN_TICK <= 1000000000, "Having tick greater than second may cause math overlapping");
