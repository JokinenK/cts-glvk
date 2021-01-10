#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSemaphore* CtsSemaphore;
typedef struct CtsSemaphoreCreateInfo {
    CtsFlags flags;
} CtsSemaphoreCreateInfo;

#ifdef __cplusplus
}
#endif