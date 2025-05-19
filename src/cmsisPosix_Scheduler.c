#define _GNU_SOURCE
#include "cmsis_os2.h"
#include "cmsisPosix_Config.h"

extern osStatus_t cp_threadInitSystem();

osStatus_t osKernelInitialize (void)
{
    return cp_threadInitSystem();
}