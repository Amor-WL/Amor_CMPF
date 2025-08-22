#ifndef AMOR_SHARED_MEMORY_H
#define AMOR_SHARED_MEMORY_H

#include "../amor.h"
#include "../log/amor_log.h"
#include "../utils/amor_utils.h"

#ifdef AMOR_PLATFORM_WINDOWS
#include <windows.h>
#endif

// 共享内存标志
enum {
    AMOR_SHM_CREATE = 1,
    AMOR_SHM_EXCL = 2,
    AMOR_SHM_RDONLY = 4
};

typedef struct amor_shared_memory_s {
    void *addr;
    size_t size;
    char *name;
    int flags;
#ifdef AMOR_PLATFORM_WINDOWS
    HANDLE handle;
#else
    int fd;
#endif
} amor_shared_memory_t;

// 共享内存API
#ifdef __cplusplus
extern "C" {
#endif

extern int amor_shared_memory_init(void);
extern void amor_shared_memory_cleanup(void);
extern amor_shared_memory_t *amor_shared_memory_open(const char *name, size_t size, int flags);
extern int amor_shared_memory_close(amor_shared_memory_t *shm);
extern int amor_shared_memory_unlink(const char *name);
extern void *amor_shared_memory_map(amor_shared_memory_t *shm);
extern int amor_shared_memory_unmap(amor_shared_memory_t *shm);
extern int amor_shared_memory_lock(amor_shared_memory_t *shm);
extern int amor_shared_memory_unlock(amor_shared_memory_t *shm);

#ifdef __cplusplus
}
#endif

#endif // AMOR_SHARED_MEMORY_H