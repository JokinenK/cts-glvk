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
    const CtsClearAttachment* attachments;
    uint32_t rectCount;
    const CtsClearRect* rects;
} CtsCmdClearAttachments;

#ifdef __cplusplus
}
#endif