#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdClearAttachments {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    uint32_t attachmentCount;
    const CtsClearAttachment* pAttachments;
    uint32_t rectCount;
    const CtsClearRect* pRects;
} CtsCmdClearAttachments;

#ifdef __cplusplus
}
#endif