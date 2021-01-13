#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyBufferToImage {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsBuffer srcBuffer;
    CtsImage dstImage;
    CtsImageLayout dstImageLayout;
    uint32_t regionCount;
    const CtsBufferImageCopy* pRegions;
} CtsCmdCopyBufferToImage;

#ifdef __cplusplus
}
#endif