#pragma once

#include <cts/typedefs/attachment_description.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/render_pass_create_flags.h>
#include <cts/typedefs/subpass_description.h>
#include <cts/typedefs/subpass_dependency.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsRenderPass* CtsRenderPass;
typedef struct CtsRenderPassCreateInfo {
    CtsStructureType sType;
    const void* next;
    CtsRenderPassCreateFlags flags;
    uint32_t attachmentCount;
    const CtsAttachmentDescription* attachments;
    uint32_t subpassCount;
    const CtsSubpassDescription* subpasses;
    uint32_t dependencyCount;
    const CtsSubpassDependency* dependencies;
} CtsRenderPassCreateInfo;

#ifdef __cplusplus
}
#endif