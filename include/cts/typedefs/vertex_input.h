#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsVertexInputAttributeDescription {
    uint32_t    location;
    uint32_t    binding;
    CtsFormat   format;
    uint32_t    offset;
} CtsVertexInputAttributeDescription;

typedef struct CtsVertexInputBindingDescription {
    uint32_t            binding;
    uint32_t            stride;
    CtsVertexInputRate  inputRate;
} CtsVertexInputBindingDescription;

#ifdef __cplusplus
}
#endif