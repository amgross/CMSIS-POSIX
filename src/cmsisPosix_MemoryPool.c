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

// Structure to hold POSIX memory pool and CMSIS attributes
typedef struct
{
    const char *name;           // Name of memory pool
    uint32_t block_count;       // Total number of blocks
    uint32_t block_size;        // Size of each block
    uint32_t padded_block_size; // Padded size of each block
    uint8_t *arena;             // Pre-allocated arena for memory blocks
    sem_t sem;                  // Semaphore to keep track of block usage
    pthread_mutex_t mutex;      // Mutex for used_flags
    uint8_t *used_flags;        // Array of status flags (none-zero if block is used)
} cp_memPoolData_t;

osMemoryPoolId_t osMemoryPoolNew(uint32_t block_count, uint32_t block_size, const osMemoryPoolAttr_t *attr)
{
    if ((block_count == 0) || (block_size == 0)) {
        return NULL;
    }

    // Calculate padded block size required to ensure each block starts at an aligned address
    uint32_t align_size = sizeof(max_align_t);
    uint32_t padded_block_size = align_size * ((block_size + align_size - 1) / align_size);

    cp_memPoolData_t *mem_pool = malloc(sizeof(cp_memPoolData_t));
    if (mem_pool == NULL) {
        return NULL;
    }

    if (sem_init(&mem_pool->sem, 0, block_count) != 0) {
        free(mem_pool);
        return NULL;
    }

    if (pthread_mutex_init(&mem_pool->mutex, NULL) != 0) {
        sem_destroy(&mem_pool->sem);
        free(mem_pool);
        return NULL;
    }

    mem_pool->name = (attr ? attr->name : NULL);
    mem_pool->block_count = block_count;
    mem_pool->block_size = block_size;
    mem_pool->padded_block_size = padded_block_size;
    mem_pool->used_flags = calloc(block_count, sizeof(uint8_t));

    // Instead of using cb_mem, an arena of padded blocks is allocated to ensure memory alignment for each block
    mem_pool->arena = malloc(block_count * padded_block_size);

    if ((mem_pool->arena == NULL) || (mem_pool->used_flags == NULL)) {
        osMemoryPoolDelete(mem_pool);
        return NULL;
    }

    return (osMemoryPoolId_t)mem_pool;
}

const char *osMemoryPoolGetName (osMemoryPoolId_t mp_id)
{
    cp_memPoolData_t *mem_pool = (cp_memPoolData_t *)mp_id;
    
    if (mem_pool == NULL) {
        NULL;
    }

    return mem_pool->name;
}

void *osMemoryPoolAlloc(osMemoryPoolId_t mp_id, uint32_t timeout)
{
    cp_memPoolData_t *mem_pool = (cp_memPoolData_t *)mp_id;
    
    if (mem_pool == NULL) {
        return NULL;
    }

    // Lock the semaphore
    int posix_ret;
    if (timeout == 0) {
        // Try to lock without waiting
        posix_ret = sem_trywait(&mem_pool->sem);
    } else if (timeout == osWaitForever) {
        // Block indefinitely
        posix_ret = sem_wait(&mem_pool->sem);
    } else {
        struct timespec ts;
        cp_timeoutToTimespec(timeout, &ts);

        // Wait for the required duration
        posix_ret = sem_timedwait(&mem_pool->sem, &ts);
    }

    if (posix_ret != 0) {
        return NULL;
    }

    // Return first unused block
    void *block = NULL;
    pthread_mutex_lock(&mem_pool->mutex);
    for (uint32_t idx = 0;  idx < mem_pool->block_count;  idx++) {
        if (!mem_pool->used_flags[idx]) {
            // Mark block as used
            mem_pool->used_flags[idx] = 1;
            block = mem_pool->arena + (idx * mem_pool->padded_block_size);
            break;
        }
    }
    pthread_mutex_unlock(&mem_pool->mutex);
    return block;
}

osStatus_t osMemoryPoolFree(osMemoryPoolId_t mp_id, void *block)
{
    cp_memPoolData_t *mem_pool = (cp_memPoolData_t *)mp_id;
    
    if (mem_pool == NULL) {
        return osErrorParameter;
    }

    osStatus_t status = osErrorParameter;

    // Look for matching memory pointer
    pthread_mutex_lock(&mem_pool->mutex);
    for (uint32_t idx = 0;  idx < mem_pool->block_count;  idx++) {
        if (block == (mem_pool->arena + (idx * mem_pool->padded_block_size)))
        {
            // Mark block as unused
            if (mem_pool->used_flags[idx]) {
                mem_pool->used_flags[idx] = 0;
                status = osOK;
                break;
            }
        }
    }
    pthread_mutex_unlock(&mem_pool->mutex);

    // Unlock the semaphore if the block was freed
    if (status == osOK) {
        sem_post(&mem_pool->sem);
    }

    return status;
}

uint32_t osMemoryPoolGetCapacity(osMemoryPoolId_t mp_id)
{
    cp_memPoolData_t *mem_pool = (cp_memPoolData_t *)mp_id;
    
    if (mem_pool == NULL) {
        return 0;
    }

    return mem_pool->block_count;
}

uint32_t osMemoryPoolGetBlockSize(osMemoryPoolId_t mp_id)
{
    cp_memPoolData_t *mem_pool = (cp_memPoolData_t *)mp_id;
    
    if (mem_pool == NULL) {
        return 0;
    }

    return mem_pool->block_size;
}

uint32_t osMemoryPoolGetCount(osMemoryPoolId_t mp_id)
{
    cp_memPoolData_t *mem_pool = (cp_memPoolData_t *)mp_id;
    
    if (mem_pool == NULL) {
        return 0;
    }

    return mem_pool->block_count - osMemoryPoolGetSpace(mp_id);
}

uint32_t osMemoryPoolGetSpace(osMemoryPoolId_t mp_id)
{
    cp_memPoolData_t *mem_pool = (cp_memPoolData_t *)mp_id;
    
    if (mem_pool == NULL) {
        return 0;
    }

    int space = 0;
    if ((sem_getvalue(&mem_pool->sem, &space) != 0) || (space < 0)) {
        space = 0;
    }
    return space;
}

osStatus_t osMemoryPoolDelete(osMemoryPoolId_t mp_id)
{
    cp_memPoolData_t *mem_pool = (cp_memPoolData_t *)mp_id;
    
    if (mem_pool == NULL) {
        return osErrorParameter;
    }

    sem_destroy(&mem_pool->sem);
    pthread_mutex_destroy(&mem_pool->mutex);
    free(mem_pool->arena);
    free(mem_pool->used_flags);
    free(mem_pool);
    return osOK;
}