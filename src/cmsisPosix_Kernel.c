#define _GNU_SOURCE
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"
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