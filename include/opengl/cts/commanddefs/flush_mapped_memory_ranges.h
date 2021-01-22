#pragma once

#include <stdint.h>
#include <cts/commanddefs/cmd_base.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFlushMappedMemoryRanges {
    CtsCmdBase base;
    CtsDevice device;
    uint32_t memoryRangeCount;
    const CtsMappedMemoryRange* pMemoryRanges;
    CtsResult* pResult;
} CtsFlushMappedMemoryRanges;

#ifdef __cplusplus
}
#endif