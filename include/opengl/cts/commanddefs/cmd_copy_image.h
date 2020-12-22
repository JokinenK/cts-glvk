#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdCopyImage {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsImage srcImage;
    CtsImageLayout srcImageLayout;
    CtsImage dstImage;
    CtsImageLayout dstImageLayout;
    uint32_t regionCount;
    const CtsImageCopy* regions;
} CtsCmdCopyImage;

#ifdef __cplusplus
}
#endif