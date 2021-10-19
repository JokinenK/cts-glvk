#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/trampoline.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBase {
    const struct CtsCmdBase* pNext;
    CtsCmdMetadata* pMetadata;
    size_t extraDataLen;
} CtsCmdBase;

#ifdef __cplusplus
}
#endif