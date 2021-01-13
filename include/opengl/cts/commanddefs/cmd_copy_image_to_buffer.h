#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyImageToBuffer {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsImage srcImage;
    CtsImageLayout srcImageLayout;
    CtsBuffer dstBuffer;
    uint32_t regionCount;
    const CtsBufferImageCopy* pRegions;
} CtsCmdCopyImageToBuffer;

#ifdef __cplusplus
}
#endif