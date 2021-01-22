#include <stddef.h>
#include <string.h>
#include <cts/render_pass.h>
#include <private/render_pass_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateRenderPass(
    CtsDevice device,
    const CtsRenderPassCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsRenderPass* pRenderPass
) {
    (void) device;

    CtsRenderPass renderPass = ctsAllocation(
        pAllocator,
        sizeof(struct CtsRenderPassImpl),
        alignof(struct CtsRenderPassImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    renderPass->attachmentCount = pCreateInfo->attachmentCount;
    renderPass->pAttachments = ctsAllocation(
        pAllocator,
        sizeof(CtsAttachmentDescription) * pCreateInfo->attachmentCount,
        alignof(CtsAttachmentDescription),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    renderPass->pDrawBuffers = ctsAllocation(
        pAllocator,
        sizeof(GLenum) * pCreateInfo->attachmentCount,
        alignof(GLenum),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass->pAttachments == NULL || renderPass->pDrawBuffers == NULL) {
        ctsDestroyRenderPass(device, renderPass, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        renderPass->pAttachments[i] = pCreateInfo->pAttachments[i];
    }

    renderPass->dependencyCount = pCreateInfo->dependencyCount;
    renderPass->pDependencies = NULL;

    if (pCreateInfo->dependencyCount > 0) {
        renderPass->pDependencies = ctsAllocation(
            pAllocator,
            sizeof(CtsSubpassDependency) * pCreateInfo->dependencyCount,
            alignof(CtsSubpassDependency),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (renderPass->pDependencies == NULL) {
            ctsDestroyRenderPass(device, renderPass, pAllocator);
            return CTS_ERROR_OUT_OF_HOST_MEMORY;
        }

        for (uint32_t i = 0; i < pCreateInfo->dependencyCount; ++i) {
            renderPass->pDependencies[i] = pCreateInfo->pDependencies[i];
        }
    }

    renderPass->subpassCount = pCreateInfo->subpassCount;
    renderPass->pSubpasses = NULL;

    if (pCreateInfo->subpassCount > 0) {
        renderPass->pSubpasses = ctsAllocation(
            pAllocator,
            sizeof(CtsGlSubpassDescription) * pCreateInfo->subpassCount,
            alignof(CtsGlSubpassDescription),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (renderPass->pSubpasses == NULL) {
            ctsDestroyRenderPass(device, renderPass, pAllocator);
            return CTS_ERROR_OUT_OF_HOST_MEMORY;
        }

        for (uint32_t i = 0; i < pCreateInfo->subpassCount; ++i) {
            const CtsSubpassDescription* source = &pCreateInfo->pSubpasses[i];
            CtsGlSubpassDescription* dest = &renderPass->pSubpasses[i];

            size_t inputAttachmentSize      = (source->inputAttachmentCount * sizeof(CtsAttachmentReference));
            size_t colorAttachmentSize      = (source->colorAttachmentCount * sizeof(CtsAttachmentReference));
            size_t resolveAttachmentSize    = (source->colorAttachmentCount * sizeof(CtsAttachmentReference));
            size_t depthStencilSize         = (sizeof(CtsAttachmentReference));
            size_t preserveAttachmentSize   = (source->preserveAttachmentCount * sizeof(uint32_t));
            size_t extraDataLen             = (inputAttachmentSize + colorAttachmentSize +  resolveAttachmentSize +  depthStencilSize +  preserveAttachmentSize);

            dest->extraDataBase = ctsAllocation(
                pAllocator,
                sizeof(char) * extraDataLen,
                alignof(char),
                CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
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

            if (source->pInputAttachments != NULL) {
                memcpy(dest->pInputAttachments, dest->pInputAttachments, inputAttachmentSize);
            } else {
                dest->pInputAttachments = NULL;
            }

            if (source->pColorAttachments != NULL) {
                memcpy(dest->pColorAttachments, source->pColorAttachments, colorAttachmentSize);
            } else {
                dest->pColorAttachments = NULL;
            }

            if (source->pResolveAttachments != NULL) {
                memcpy(dest->pResolveAttachments, source->pResolveAttachments, resolveAttachmentSize);
            } else {
                dest->pResolveAttachments = NULL;
            }

            if (source->pDepthStencilAttachment != NULL) {
                memcpy(dest->pDepthStencilAttachment, source->pDepthStencilAttachment, depthStencilSize);
            } else {
                dest->pDepthStencilAttachment = NULL;
            }

            if (source->pPreserveAttachments != NULL) {
                memcpy(dest->pPreserveAttachments, source->pPreserveAttachments, preserveAttachmentSize);
            } else {
                dest->pPreserveAttachments = NULL;
            }
        }
    }

    *pRenderPass = renderPass;
    return CTS_SUCCESS;
}

void ctsDestroyRenderPass(
    CtsDevice device,
    CtsRenderPass renderPass,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;

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
