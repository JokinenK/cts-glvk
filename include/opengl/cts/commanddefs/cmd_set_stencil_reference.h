#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdSetStencilReference {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsStencilFaceFlags faceMask;
    uint32_t reference;
} CtsCmdSetStencilReference;

#ifdef __cplusplus
}
#endif