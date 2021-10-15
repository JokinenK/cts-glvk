#pragma once

#include "vulkan/vulkan_core.h"
#include "cts/commanddefs/command_types.h"
#include "cts/trampoline.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBase {
    CtsCommandType type;
    const struct CtsCmdBase* pNext;
} CtsCmdBase;

#ifdef __cplusplus
}
#endif