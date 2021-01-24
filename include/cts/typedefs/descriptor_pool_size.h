#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDescriptorPoolSize {
    CtsDescriptorType   type;
    uint32_t            descriptorCount;
} CtsDescriptorPoolSize;

#ifdef __cplusplus
}
#endif