#pragma once

#include <stdint.h>
#include <cts/typedefs/attachment_reference.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/subpass_description_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsSubpassDescription {
    CtsSubpassDescriptionFlags      flags;
    CtsPipelineBindPoint            pipelineBindPoint;
    uint32_t                        inputAttachmentCount;
    const CtsAttachmentReference*   pInputAttachments;
    uint32_t                        colorAttachmentCount;
    const CtsAttachmentReference*   pColorAttachments;
    const CtsAttachmentReference*   pResolveAttachments;
    const CtsAttachmentReference*   pDepthStencilAttachment;
    uint32_t                        preserveAttachmentCount;
    const uint32_t*                 pPreserveAttachments;
} CtsSubpassDescription;

#ifdef __cplusplus
}
#endif

