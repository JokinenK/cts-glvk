#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdClearDepthStencilImage {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsImage image;
    CtsImageLayout imageLayout;
    const CtsClearDepthStencilValue* pDepthStencil;
    uint32_t rangeCount;
    const CtsImageSubresourceRange* pRanges;
} CtsCmdClearDepthStencilImage;

#ifdef __cplusplus
}
#endif