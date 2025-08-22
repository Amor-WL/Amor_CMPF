#include "../../include/shared_memory/amor_shared_memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef AMOR_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif

void amor_log_info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

// 初始化共享内存系统
int amor_shared_memory_init(void) {
    // 无需特别初始化
    amor_log_info("Shared memory initialized");
    return 0;
}

// 清理共享内存系统
void amor_shared_memory_cleanup(void) {
    // 无需特别清理
    amor_log_info("Shared memory cleaned up");
}

// 打开共享内存（打桩）
amor_shared_memory_t *amor_shared_memory_open(const char *name, size_t size, int flags) {
    amor_log_info("Shared memory opened: %s size: %d flags: %d", name, size, flags);
    return NULL;
}

// 关闭共享内存
int amor_shared_memory_close(amor_shared_memory_t *shm) {
    if (!shm) {
        return -1;
    }

    // 先解除映射
    if (shm->addr) {
        amor_shared_memory_unmap(shm);
    }

#ifdef AMOR_PLATFORM_WINDOWS
    // Windows平台实现
    if (shm->handle != NULL) {
        CloseHandle(shm->handle);
        shm->handle = NULL;
    }
#else
    // Linux平台实现
    if (shm->fd != -1) {
        close(shm->fd);
        shm->fd = -1;
    }
#endif

    if (shm->name) {
        free(shm->name);
        shm->name = NULL;
    }

    free(shm);
    return 0;
}

// 删除共享内存（打桩）
int amor_shared_memory_unlink(const char *name) {
    (void)name; // 避免未使用参数警告

#ifdef AMOR_PLATFORM_WINDOWS
    // Windows平台实现（打桩）
    return 0;
#else
    // Linux平台实现（打桩）
    return 0;
#endif
}

// 映射共享内存（打桩）
void *amor_shared_memory_map(amor_shared_memory_t *shm) {
    if (!shm) {
        return NULL;
    }

    // 打桩实现，返回一个模拟的地址
    shm->addr = malloc(shm->size);
    return shm->addr;
}

// 解除映射（打桩）
int amor_shared_memory_unmap(amor_shared_memory_t *shm) {
    if (!shm || !shm->addr) {
        return -1;
    }

    // 打桩实现，释放内存
    free(shm->addr);
    shm->addr = NULL;
    return 0;
}

// 锁定共享内存（打桩）
int amor_shared_memory_lock(amor_shared_memory_t *shm) {
    (void)shm; // 避免未使用参数警告
    return 0;
}

// 解锁共享内存（打桩）
int amor_shared_memory_unlock(amor_shared_memory_t *shm) {
    (void)shm; // 避免未使用参数警告
    return 0;
}