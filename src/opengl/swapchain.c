#include <stdint.h>
#include <string.h>
#include <cts/swapchain.h>
#include <cts/fullscreen_texture.h>
#include <private/device_private.h>
#include <private/fence_private.h>
#include <private/image_private.h>
#include <private/swapchain_private.h>
#include <private/surface_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSwapchain(
    CtsDevice device,
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

    swapchain->device = device;
    swapchain->entryCount = pCreateInfo->minImageCount;
    swapchain->pEntries = ctsAllocation(
        pAllocator,
        sizeof(CtsSwapchainEntry) * swapchain->entryCount,
        alignof(CtsSwapchainEntry),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (swapchain->pEntries == NULL) {
        ctsDestroySwapchain(device, swapchain, pAllocator);
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsImageCreateInfo imageCreateInfo;
    imageCreateInfo.sType = CTS_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = NULL;
    imageCreateInfo.arrayLayers = pCreateInfo->imageArrayLayers;
    imageCreateInfo.extent.width = pCreateInfo->imageExtent.width;
    imageCreateInfo.extent.height = pCreateInfo->imageExtent.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.imageFormat = pCreateInfo->imageFormat;
    imageCreateInfo.imageType = CTS_IMAGE_TYPE_2D;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.queueFamilyIndexCount = pCreateInfo->queueFamilyIndexCount;
    imageCreateInfo.pQueueFamilyIndices = pCreateInfo->pQueueFamilyIndices;
    imageCreateInfo.samples = CTS_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.usage = pCreateInfo->imageUsage;

    for (uint32_t i = 0; i < pCreateInfo->minImageCount; ++i) {
        swapchain->pEntries[i].fence = NULL;
        swapchain->pEntries[i].semaphore = NULL;
        ctsCreateImageImpl(device, &imageCreateInfo, pAllocator, &swapchain->pEntries[i].image);
    }

    ctsInitFSTextureHelper();

    device->surface = pCreateInfo->surface;
    ctsConditionVariableWakeAll(device->init.conditionVariable);

    *pSwapchain = swapchain;

    return CTS_SUCCESS;
}

CtsResult ctsGetSwapchainImages(
    CtsDevice device,
    CtsSwapchain swapchain,
    uint32_t* pSwapchainImageCount,
    CtsImage* pSwapchainImages
) {
    if (pSwapchainImageCount != NULL) {
        *pSwapchainImageCount = swapchain->entryCount;
    }

    if (pSwapchainImages != NULL) {
        for (uint32_t i = 0; i < swapchain->entryCount; ++i) {
            pSwapchainImages[i] = swapchain->pEntries[i].image;
        }
    }

    return CTS_SUCCESS;
}

CtsResult ctsAcquireNextImage(
    CtsDevice device,
    CtsSwapchain swapchain,
    uint64_t timeout,
    CtsSemaphore semaphore,
    CtsFence fence,
    uint32_t* pImageIndex
) {
    (void) device;
    
    // TODO: Handle timeout cases
    for (uint32_t i = 0; i < swapchain->entryCount; ++i) {
        CtsSwapchainEntry* entry = &swapchain->pEntries[i];

        if (entry->free) {
            entry->free = false;
            entry->fence = fence;
            entry->semaphore = semaphore;
            *pImageIndex = i;
            return CTS_SUCCESS;
        }
    }

    return CTS_NOT_READY;
}

CtsResult ctsQueuePresent(
    CtsQueue queue,
    const CtsPresentInfo* pPresentInfo
) {
    ctsWaitSemaphores(pPresentInfo->waitSemaphoreCount, pPresentInfo->pWaitSemaphores);

    for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i) {
        uint32_t imageIndex = pPresentInfo->pImageIndices[i];
        CtsSwapchain swapchain = pPresentInfo->pSwapchains[i];

        CtsDevice device = swapchain->device;
        CtsSwapchainEntry* entry = &swapchain->pEntries[imageIndex];

        if (entry->fence != NULL) {
            ctsWaitForFencesImpl(device, 1, &entry->fence, true, UINT64_MAX);
        }
        
        ctsDrawFSTexture(device, entry->image);
        ctsSurfaceSwapBuffers(device->surface);

        if (entry->semaphore != NULL) {
            ctsSignalSemaphores(1, &entry->semaphore);
        }
    }

    return CTS_SUCCESS;
}

void ctsDestroySwapchain(
    CtsDevice device,
    CtsSwapchain swapchain,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;
    
    if (swapchain != NULL) {
        for (uint32_t i = 0; i < swapchain->entryCount; ++i) {
            if (swapchain->pEntries[i].image != NULL) {
                ctsDestroyImageImpl(device, swapchain->pEntries[i].image, pAllocator);
            }
        }

        ctsFree(pAllocator, swapchain->pEntries);
        ctsFree(pAllocator, swapchain);
        ctsCleanupFSTextureHelper();
    }
}

#ifdef __cplusplus
}
#endif