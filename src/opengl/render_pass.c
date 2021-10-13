#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <cts/render_pass.h>
#include <cts/allocator.h>
#include <cts/util/align.h>
#include <private/private.h>
#include <private/render_pass_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateRenderPass(
    VkDevice deviceHandle,
    const VkRenderPassCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    struct CtsRenderPass* renderPass = ctsAllocation(
        pAllocator,
        sizeof(struct CtsRenderPass),
        alignof(struct CtsRenderPass),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    renderPass->attachmentCount = pCreateInfo->attachmentCount;
    renderPass->pAttachments = ctsAllocation(
        pAllocator,
        sizeof(VkAttachmentDescription) * pCreateInfo->attachmentCount,
        alignof(VkAttachmentDescription),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    renderPass->pDrawBuffers = ctsAllocation(
        pAllocator,
        sizeof(GLenum) * pCreateInfo->attachmentCount,
        alignof(GLenum),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass->pAttachments == NULL || renderPass->pDrawBuffers == NULL) {
        ctsDestroyRenderPass(deviceHandle, CtsRenderPassToHandle(renderPass), pAllocator);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        renderPass->pAttachments[i] = pCreateInfo->pAttachments[i];
    }

    renderPass->dependencyCount = pCreateInfo->dependencyCount;
    renderPass->pDependencies = NULL;

    if (pCreateInfo->dependencyCount > 0) {
        renderPass->pDependencies = ctsAllocation(
            pAllocator,
            sizeof(VkSubpassDependency) * pCreateInfo->dependencyCount,
            alignof(VkSubpassDependency),
            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (renderPass->pDependencies == NULL) {
            ctsDestroyRenderPass(deviceHandle, CtsRenderPassToHandle(renderPass), pAllocator);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }

        for (uint32_t i = 0; i < pCreateInfo->dependencyCount; ++i) {
            renderPass->pDependencies[i] = pCreateInfo->pDependencies[i];
        }
    }

    renderPass->subpassCount = pCreateInfo->subpassCount;
    renderPass->pSubpasses = NULL;

    VkAttachmentReference unusedAttachment = {
        .attachment = VK_ATTACHMENT_UNUSED,
        .layout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    if (pCreateInfo->subpassCount > 0) {
        renderPass->pSubpasses = ctsAllocation(
            pAllocator,
            sizeof(CtsGlSubpassDescription) * pCreateInfo->subpassCount,
            alignof(CtsGlSubpassDescription),
            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (renderPass->pSubpasses == NULL) {
            ctsDestroyRenderPass(deviceHandle, CtsRenderPassToHandle(renderPass), pAllocator);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }

        for (uint32_t i = 0; i < pCreateInfo->subpassCount; ++i) {
            const VkSubpassDescription* source = &pCreateInfo->pSubpasses[i];
            CtsGlSubpassDescription* dest = &renderPass->pSubpasses[i];

            size_t inputAttachmentSize      = (source->inputAttachmentCount * sizeof(VkAttachmentReference));
            size_t colorAttachmentSize      = (source->colorAttachmentCount * sizeof(VkAttachmentReference));
            size_t resolveAttachmentSize    = (source->colorAttachmentCount * sizeof(VkAttachmentReference));
            size_t depthStencilSize         = (sizeof(VkAttachmentReference));
            size_t preserveAttachmentSize   = (source->preserveAttachmentCount * sizeof(uint32_t));
            size_t extraDataLen             = (inputAttachmentSize + colorAttachmentSize +  resolveAttachmentSize +  depthStencilSize +  preserveAttachmentSize);

            dest->extraDataBase = ctsAllocation(
                pAllocator,
                sizeof(char) * extraDataLen,
                alignof(char),
                VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
            );

            dest->flags                     = source->flags;
            dest->pipelineBindPoint         = source->pipelineBindPoint;
            dest->inputAttachmentCount      = source->inputAttachmentCount;
            dest->colorAttachmentCount      = source->colorAttachmentCount;
            dest->preserveAttachmentCount   = source->preserveAttachmentCount;
            dest->pInputAttachments         = (void*)((char*)dest->extraDataBase);
            dest->pColorAttachments         = (void*)((char*)dest->pInputAttachments + inputAttachmentSize);
            dest->pResolveAttachments       = (void*)((char*)dest->pColorAttachments + colorAttachmentSize);
            dest->pDepthStencilAttachment   = (void*)((char*)dest->pResolveAttachments + resolveAttachmentSize);
            dest->pPreserveAttachments      = (void*)((char*)dest->pDepthStencilAttachment + depthStencilSize);

            bool hasInputAttachments = source->pInputAttachments != NULL;
            bool hasColorAttachments = source->pColorAttachments != NULL;
            bool hasResolveAttachments = source->pResolveAttachments != NULL;
            bool hasDepthStencilAttachment = source->pDepthStencilAttachment != NULL;
            bool hasPreserveAttachments = source->pPreserveAttachments != NULL;

            if (hasInputAttachments) {
                for (uint32_t i = 0; i < source->inputAttachmentCount; ++i) {
                    dest->pInputAttachments[i] = dest->pInputAttachments[i];
                }
            } else {
                dest->pInputAttachments = NULL;
            }

            if (hasColorAttachments || hasResolveAttachments) {
                for (uint32_t i = 0; i < source->colorAttachmentCount; ++i) {
                    dest->pColorAttachments[i] = hasColorAttachments
                        ? source->pColorAttachments[i]
                        : unusedAttachment;

                    dest->pResolveAttachments[i] = hasResolveAttachments
                        ? source->pResolveAttachments[i]
                        : unusedAttachment;
                }
            } else {
                dest->pColorAttachments = NULL;
                dest->pResolveAttachments = NULL;
            }

            if (hasDepthStencilAttachment) {
                (*dest->pDepthStencilAttachment) = (*source->pDepthStencilAttachment);
            } else {
                dest->pDepthStencilAttachment = NULL;
            }

            if (hasPreserveAttachments) {
                for (uint32_t i = 0; i < source->preserveAttachmentCount; ++i) {
                    dest->pPreserveAttachments[i] = source->pPreserveAttachments[i];
                }
            } else {
                dest->pPreserveAttachments = NULL;
            }
        }
    }

    *pRenderPass = CtsRenderPassToHandle(renderPass);
    return VK_SUCCESS;
}

void ctsDestroyRenderPass(
    VkDevice deviceHandle,
    VkRenderPass renderPassHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;

    struct CtsRenderPass* renderPass = CtsRenderPassFromHandle(renderPassHandle);

    if (renderPass != NULL) {
        if (renderPass->pAttachments != NULL) {
            ctsFree(pAllocator, renderPass->pAttachments);
        }

        if (renderPass->pDrawBuffers != NULL) {
            ctsFree(pAllocator, renderPass->pDrawBuffers);
        }

        if (renderPass->pDependencies != NULL) {
            ctsFree(pAllocator, renderPass->pDependencies);
        }

        if (renderPass->pSubpasses != NULL) {
            for (uint32_t i = 0; i < renderPass->subpassCount; ++i) {
                ctsFree(pAllocator, renderPass->pSubpasses[i].extraDataBase);
            }
        
            ctsFree(pAllocator, renderPass->pSubpasses);
        }

        ctsFree(pAllocator, renderPass);
    }
}

#ifdef __cplusplus
}
#endif
