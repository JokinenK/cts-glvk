#pragma once

#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsRenderPass {
    CtsRenderPassCreateFlags flags;
    uint32_t attachmentCount;
    CtsAttachmentDescription* attachments;
    uint32_t subpassCount;
    CtsSubpassDescription* subpasses;
    uint32_t dependencyCount;
    CtsSubpassDependency* dependencies;
};

#ifdef __cplusplus
}
#endif