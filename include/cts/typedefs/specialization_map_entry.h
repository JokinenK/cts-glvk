#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSpecializationMapEntry {
    uint32_t    constantID;
    uint32_t    offset;
    size_t      size;
} CtsSpecializationMapEntry;

#ifdef __cplusplus
}
#endif