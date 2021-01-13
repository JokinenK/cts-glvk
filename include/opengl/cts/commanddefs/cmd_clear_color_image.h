#pragma once

#include <cts/types.h>
#include <cts/commanddefs/cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsCmdClearColorImage {
    CtsCmdBase base;
    CtsCommandBuffer commandBuffer;
    CtsImage image;
    CtsImageLayout imageLayout;
    const CtsClearColorValue* pColor;
    uint32_t rangeCount;
    const CtsImageSubresourceRange* pRanges;
} CtsCmdClearColorImage;

#ifdef __cplusplus
}
#endif