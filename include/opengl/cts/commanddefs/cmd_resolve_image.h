#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdResolveImage {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsImage srcImage;
    CtsImageLayout srcImageLayout;
    CtsImage dstImage;
    CtsImageLayout dstImageLayout;
    uint32_t regionCount;
    CtsImageResolve* pRegions;
} CtsCmdResolveImage;

#ifdef __cplusplus
}
#endif