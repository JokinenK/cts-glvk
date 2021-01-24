#pragma once

#include <cts/typedefs/attachment_description.h>
#include <cts/typedefs/clear_value.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/framebuffer.h>
#include <cts/typedefs/render_pass.h>
#include <cts/typedefs/rect.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsRenderPassBeginInfo {
    CtsStructureType        sType;
    const void*             pNext;
    CtsRenderPass           renderPass;
    CtsFramebuffer          framebuffer;
    CtsRect2D               renderArea;
    uint32_t                clearValueCount;
    const CtsClearValue*    pClearValues;
} CtsRenderPassBeginInfo;

#ifdef __cplusplus
}
#endif