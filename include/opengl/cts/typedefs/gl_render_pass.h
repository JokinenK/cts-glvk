#pragma once

#include <stdint.h>
#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlSubpassDescription {
    CtsSubpassDescriptionFlags flags;
    CtsPipelineBindPoint pipelineBindPoint;
    uint32_t inputAttachmentCount;
    CtsAttachmentReference* pInputAttachments;
    uint32_t colorAttachmentCount;
    CtsAttachmentReference* pColorAttachments;
    CtsAttachmentReference* pResolveAttachments;
    CtsAttachmentReference* pDepthStencilAttachment;
    uint32_t preserveAttachmentCount;
    uint32_t* pPreserveAttachments;
    void* extraDataBase;
} CtsGlSubpassDescription;

#ifdef __cplusplus
}
#endif