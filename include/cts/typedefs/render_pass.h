#pragma once

#include <cts/typedefs/attachment_description.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/render_pass_create_flags.h>
#include <cts/typedefs/subpass_description.h>
#include <cts/typedefs/subpass_dependency.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsRenderPassImpl* CtsRenderPass;
typedef struct CtsRenderPassCreateInfo {
    CtsStructureType                sType;
    const void*                     pNext;
    CtsRenderPassCreateFlags        flags;
    uint32_t                        attachmentCount;
    const CtsAttachmentDescription* pAttachments;
    uint32_t                        subpassCount;
    const CtsSubpassDescription*    pSubpasses;
    uint32_t                        dependencyCount;
    const CtsSubpassDependency*     pDependencies;
} CtsRenderPassCreateInfo;

#ifdef __cplusplus
}
#endif