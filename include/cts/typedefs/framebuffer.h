#pragma once

#include <stdint.h>
#include <cts/typedefs/framebuffer_create_flags.h>
#include <cts/typedefs/render_pass.h>
#include <cts/typedefs/image_view.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsFramebuffer* CtsFramebuffer;

typedef struct CtsFramebufferCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsFramebufferCreateFlags flags;
    CtsRenderPass renderPass;
    uint32_t attachmentCount;
    const CtsImageView* attachments;
    uint32_t width;
    uint32_t height;
    uint32_t layers;
} CtsFramebufferCreateInfo;

#ifdef __cplusplus
}
#endif