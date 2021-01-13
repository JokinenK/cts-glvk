#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdBeginRenderPass {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    const CtsRenderPassBeginInfo* pRenderPassBegin;
    CtsSubpassContents contents;
} CtsCmdBeginRenderPass;

#ifdef __cplusplus
}
#endif