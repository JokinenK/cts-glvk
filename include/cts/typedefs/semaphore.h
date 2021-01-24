#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSemaphoreImpl* CtsSemaphore;
typedef struct CtsSemaphoreCreateInfo {
    CtsStructureType    sType;
    const void*         pNext;
    CtsFlags            flags;
} CtsSemaphoreCreateInfo;

#ifdef __cplusplus
}
#endif