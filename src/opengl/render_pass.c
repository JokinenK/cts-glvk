#include <stddef.h>
#include <cts/render_pass.h>
#include <private/render_pass_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateRenderPass(
    CtsDevice pDevice,
    const CtsRenderPassCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsRenderPass* pRenderPass
) {
    (void) pDevice;

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
    renderPass->attachments = ctsAllocation(
        pAllocator,
        sizeof(CtsAttachmentDescription) * pCreateInfo->attachmentCount,
        alignof(CtsAttachmentDescription),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass->attachments == NULL) {
        ctsDestroyRenderPass(pDevice, renderPass, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        renderPass->attachments[i] = pCreateInfo->attachments[i];
    }

    renderPass->dependencyCount = pCreateInfo->dependencyCount;
    renderPass->dependencies = ctsAllocation(
        pAllocator,
        sizeof(CtsSubpassDescription) * pCreateInfo->dependencyCount,
        alignof(CtsSubpassDescription),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass->dependencies == NULL) {
        ctsDestroyRenderPass(pDevice, renderPass, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->dependencyCount; ++i) {
        renderPass->dependencies[i] = pCreateInfo->dependencies[i];
    }

    renderPass->subpassCount = pCreateInfo->subpassCount;
    renderPass->subpasses = ctsAllocation(
        pAllocator,
        sizeof(CtsSubpassDependency) * pCreateInfo->subpassCount,
        alignof(CtsSubpassDependency),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (renderPass->subpasses == NULL) {
        ctsDestroyRenderPass(pDevice, renderPass, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < pCreateInfo->subpassCount; ++i) {
        renderPass->subpasses[i] = pCreateInfo->subpasses[i];
    }

    *pRenderPass = renderPass;
    return CTS_SUCCESS;
}

void ctsDestroyRenderPass(
    CtsDevice pDevice,
    CtsRenderPass pRenderPass,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pRenderPass != NULL) {
        if (pRenderPass->attachments != NULL) {
            ctsFree(pAllocator, pRenderPass->attachments);
        }

        if (pRenderPass->dependencies != NULL) {
            ctsFree(pAllocator, pRenderPass->dependencies);
        }

        if (pRenderPass->subpasses != NULL) {
            ctsFree(pAllocator, pRenderPass->subpasses);
        }

        ctsFree(pAllocator, pRenderPass);
    }
}

#ifdef __cplusplus
}
#endif