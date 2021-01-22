#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsMutexImpl* CtsMutex;
typedef struct CtsMutexCreateInfo {
    char unused;
} CtsMutexCreateInfo;

#ifdef __cplusplus
}
#endif