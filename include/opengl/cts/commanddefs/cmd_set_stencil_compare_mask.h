#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetStencilCompareMask {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsStencilFaceFlags faceMask;
    uint32_t compareMask;
} CtsCmdSetStencilCompareMask;

#ifdef __cplusplus
}
#endif