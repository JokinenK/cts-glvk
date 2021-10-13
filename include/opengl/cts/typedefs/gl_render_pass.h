#pragma once

#include <stdint.h>
#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlSubpassDescription {
    VkSubpassDescriptionFlags flags;
    VkPipelineBindPoint pipelineBindPoint;
    uint32_t inputAttachmentCount;
    VkAttachmentReference* pInputAttachments;
    uint32_t colorAttachmentCount;
    VkAttachmentReference* pColorAttachments;
    VkAttachmentReference* pResolveAttachments;
    VkAttachmentReference* pDepthStencilAttachment;
    uint32_t preserveAttachmentCount;
    uint32_t* pPreserveAttachments;
    void* extraDataBase;
} CtsGlSubpassDescription;

#ifdef __cplusplus
}
#endif