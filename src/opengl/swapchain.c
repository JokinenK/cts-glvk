#include <stdint.h>
#include <string.h>
#include <cts/swapchain.h>
#include <cts/renderer_impl.h>
#include <cts/fullscreen_texture.h>
#include <private/swapchain_private.h>
#include <private/fence_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSwapchain(
    CtsDevice pDevice,
    const CtsSwapchainCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSwapchain* pSwapchain
) {
    CtsSwapchain swapchain = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSwapchain),
        alignof(struct CtsSwapchain),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (swapchain == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    (void) pCreateInfo->flags;
    //(void) pCreateInfo->imageColorSpace;
    //(void) pCreateInfo->imageSharingMode;
    //(void) pCreateInfo->preTransform;
    //(void) pCreateInfo->compositeAlpha;
    //(void) pCreateInfo->presentMode;

    swapchain->device = pDevice;
    swapchain->surface = pCreateInfo->surface;
    swapchain->entryCount = pCreateInfo->minImageCount;
    swapchain->entries = ctsAllocation(
        pAllocator,
        sizeof(CtsSwapchainEntry) * swapchain->entryCount,
        alignof(CtsSwapchainEntry),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    CtsImageCreateInfo imageCreateInfo;
    imageCreateInfo.sType = CTS_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.next = NULL;
    imageCreateInfo.arrayLayers = pCreateInfo->imageArrayLayers;
    imageCreateInfo.extent.width = pCreateInfo->imageExtent.width;
    imageCreateInfo.extent.height = pCreateInfo->imageExtent.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.imageFormat = pCreateInfo->imageFormat;
    imageCreateInfo.imageType = CTS_IMAGE_TYPE_2D;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.queueFamilyIndexCount = pCreateInfo->queueFamilyIndexCount;
    imageCreateInfo.queueFamilyIndices = pCreateInfo->queueFamilyIndices;
    imageCreateInfo.samples = CTS_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.usage = pCreateInfo->imageUsage;

    for (uint32_t i = 0; i < pCreateInfo->minImageCount; ++i) {
        swapchain->entries[i].fence = NULL;
        swapchain->entries[i].semaphore = NULL;
        ctsCreateImageImpl(pDevice, &imageCreateInfo, pAllocator, &swapchain->entries[i].image);
    }

    ctsInitFSTextureHelper();
    *pSwapchain = swapchain;   
}

CtsResult ctsGetSwapchainImages(
    CtsDevice pDevice,
    CtsSwapchain pSwapchain,
    uint32_t* pSwapchainImageCount,
    CtsImage* pSwapchainImages
) {
    if (pSwapchainImageCount != NULL) {
        *pSwapchainImageCount = pSwapchain->entryCount;
    }

    if (pSwapchainImages != NULL) {
        for (uint32_t i = 0; i < pSwapchain->entryCount; ++i) {
            pSwapchainImages[i] = pSwapchain->entries[i].image;
        }
    }
}

CtsResult ctsAcquireNextImage(
    CtsDevice pDevice,
    CtsSwapchain pSwapchain,
    uint64_t pTimeout,
    CtsSemaphore pSemaphore,
    CtsFence pFence,
    uint32_t* pImageIndex
) {
    (void) pDevice;
    
    // TODO: Handle timeout cases
    for (uint32_t i = 0; i < pSwapchain->entryCount; ++i) {
        CtsSwapchainEntry* entry = &pSwapchain->entries[i];

        if (entry->free) {
            entry->free = false;
            entry->fence = pFence;
            entry->semaphore = pSemaphore;
            *pImageIndex = i;
            return CTS_SUCCESS;
        }
    }

    return CTS_NOT_READY;
}

CtsResult ctsQueuePresent(
    CtsQueue pQueue,
    const CtsPresentInfo* pPresentInfo
) {
    // TODO: Create async wrapper
    // TODO: Set queue state
    ctsWaitSemaphores(pPresentInfo->waitSemaphoreCount, pPresentInfo->waitSemaphores);

    for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i) {
        uint32_t imageIndex = pPresentInfo->imageIndices[i];
        CtsSwapchain swapchain = pPresentInfo->swapchains[i];
        CtsSwapchainEntry* entry = &swapchain->entries[imageIndex];

        if (entry->fence != NULL) {
            ctsWaitForFencesImpl(swapchain->device, 1, &entry->fence, true, UINT64_MAX);
        }
        
        ctsDrawFSTexture(pQueue->device, entry->image);

        if (entry->semaphore != NULL) {
            ctsSignalSemaphores(1, entry->semaphore);
        }
    }
}

void ctsDestroySwapchain(
    CtsDevice pDevice,
    CtsSwapchain pSwapchain,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;
    
    if (pSwapchain != NULL) {
        for (uint32_t i = 0; i < pSwapchain->entryCount; ++i) {
            ctsDestroyImageImpl(pDevice, pSwapchain->entries[i].image, pAllocator);
        }

        ctsFree(pAllocator, pSwapchain->entries);
        ctsFree(pAllocator, pSwapchain);
        ctsCleanupFSTextureHelper();
    }
}

#ifdef __cplusplus
}
#endif