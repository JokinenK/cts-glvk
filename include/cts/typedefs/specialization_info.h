#pragma once

#include <stdint.h>
#include <cts/typedefs/specialization_map_entry.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSpecializationInfo {
    uint32_t mapEntryCount;
    const CtsSpecializationMapEntry* mapEntries;
    size_t dataSize;
    const void* data;
} CtsSpecializationInfo;

#ifdef __cplusplus
}
#endif