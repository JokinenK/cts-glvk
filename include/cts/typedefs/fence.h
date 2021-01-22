#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/fence_create_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFenceImpl* CtsFence;
typedef struct CtsFenceCreateInfo {
    CtsStructureType sType;
    const void* pNext;
    CtsFenceCreateFlags flags;
} CtsFenceCreateInfo;

#ifdef __cplusplus
}
#endif