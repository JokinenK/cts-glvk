#include <stddef.h>
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
        sizeof(struct CtsRenderPass),
        alignof(struct CtsRenderPass),
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

    if (renderPass->pAttachments == NULL) {
        ctsDestroyRenderPass(device, renderPass, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        renderPass->pAttachments[i] = pCreateInfo->pAttachments[i];
    }

    renderPass->dependencyCount = pCreateInfo->dependencyCount;
    renderPass->pDependencies = ctsAllocation(
        pAllocator,
        sizeof(CtsSubpassDescription) * pCreateInfo->dependencyCount,
        alignof(CtsSubpassDescription),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass->pDependencies == NULL) {
        ctsDestroyRenderPass(device, renderPass, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->dependencyCount; ++i) {
        renderPass->pDependencies[i] = pCreateInfo->pDependencies[i];
    }

    renderPass->subpassCount = pCreateInfo->subpassCount;
    renderPass->pSubpasses = ctsAllocation(
        pAllocator,
        sizeof(CtsSubpassDependency) * pCreateInfo->subpassCount,
        alignof(CtsSubpassDependency),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass->pSubpasses == NULL) {
        ctsDestroyRenderPass(device, renderPass, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->subpassCount; ++i) {
        renderPass->pSubpasses[i] = pCreateInfo->pSubpasses[i];
    }

    renderPass->pDrawBuffers = ctsAllocation(
        pAllocator,
        sizeof(GLenum) * pCreateInfo->attachmentCount,
        alignof(GLenum),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass->pDrawBuffers == NULL) {
        ctsDestroyRenderPass(device, renderPass, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
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

        if (renderPass->pDependencies != NULL) {
            ctsFree(pAllocator, renderPass->pDependencies);
        }

        if (renderPass->pSubpasses != NULL) {
            ctsFree(pAllocator, renderPass->pSubpasses);
        }

        if (renderPass->pDrawBuffers != NULL) {
            ctsFree(pAllocator, renderPass->pDrawBuffers);
        }

        ctsFree(pAllocator, renderPass);
    }
}

#ifdef __cplusplus
}
#endif