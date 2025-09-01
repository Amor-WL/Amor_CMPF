#ifndef CMPF_SHARED_MEMORY_H
#define CMPF_SHARED_MEMORY_H

#include "log/cmpf_log.h"
#include "utils/cmpf_utils.h"

#ifdef CMPF_PLATFORM_WINDOWS
#include <windows.h>
#endif

// 共享内存API
#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) void shared_memory_stub();

#ifdef __cplusplus
}
#endif

#endif // CMPF_SHARED_MEMORY_H