#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/command_buffer_inheritance_info.h>
#include <cts/typedefs/command_buffer_usage_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCommandBufferBeginInfo {
    CtsStructureType                        sType;
    const void*                             pNext;
    CtsCommandBufferUsageFlags              flags;
    const CtsCommandBufferInheritanceInfo*  pInheritanceInfo;
} CtsCommandBufferBeginInfo;

#ifdef __cplusplus
}
#endif