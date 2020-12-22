#pragma once

#include <stdint.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsStencilOpState {
    CtsStencilAction failOp;
    CtsStencilAction passOp;
    CtsStencilAction depthFailOp;
    CtsStencilAction compareOp;
    uint32_t compareMask;
    uint32_t writeMask;
    uint32_t reference;
} CtsStencilOpState;

#ifdef __cplusplus
}
#endif

