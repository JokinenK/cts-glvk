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
    const CtsClearDepthStencilValue* depthStencil;
    uint32_t rangeCount;
    const CtsImageSubresourceRange* ranges;
} CtsCmdClearDepthStencilImage;

#ifdef __cplusplus
}
#endif