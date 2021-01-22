#include <stdint.h>
#include <string.h>
#include <cts/swapchain.h>
#include <cts/fullscreen_texture.h>
#include <cts/commands.h>
#include <private/device_private.h>
#include <private/fence_private.h>
#include <private/image_private.h>
#include <private/physical_device_private.h>
#include <private/swapchain_private.h>
#include <private/surface_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static CtsResult validateSwapchainSurface(CtsSwapchain swapchain);

CtsResult ctsCreateSwapchain(
    CtsDevice device,
    const CtsSwapchainCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSwapchain* pSwapchain
) {
    CtsResult result;
    CtsCreateSwapchain cmd;
    cmd.base.type = CTS_COMMAND_CREATE_SWAPCHAIN;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pSwapchain = pSwapchain;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void ctsDestroySwapchain(
    CtsDevice device,
    CtsSwapchain swapchain,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroySwapchain cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_SWAPCHAIN;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.swapchain = swapchain;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsQueuePresent(
    CtsQueue queue,
    const CtsPresentInfo* pPresentInfo
) {
    CtsResult result;
    CtsQueuePresent cmd;
    cmd.base.type = CTS_COMMAND_QUEUE_PRESENT;
    cmd.base.pNext = NULL;

    cmd.queue = queue;
    cmd.pPresentInfo = pPresentInfo;
    cmd.pResult = &result;

    CtsDevice device = queue->device;
    ctsQueueDispatch(queue, &cmd.base);

    return result;
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
    uint32_t imageIndex = swapchain->nextEntry;
    swapchain->nextEntry = (swapchain->nextEntry + 1) % swapchain->entryCount;

    CtsSwapchainEntry* entry = &swapchain->pEntries[imageIndex];

    if (semaphore != NULL) {
        ctsSignalSemaphores(1, &semaphore);
    }

    if (fence != NULL) {
        ctsSignalFence(device, fence);
    }

    *pImageIndex = imageIndex;

    if (ctsWaitSemaphore(entry->semaphore, timeout) == CTS_TIMEOUT) {
        return CTS_TIMEOUT;
    }
    
    return validateSwapchainSurface(swapchain);
}

CtsResult ctsCreateSwapchainImpl(
    CtsDevice device,
    const CtsSwapchainCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSwapchain* pSwapchain
) {
    CtsSwapchain swapchain = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSwapchainImpl),
        alignof(struct CtsSwapchainImpl),
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
    swapchain->nextEntry = 0;
    swapchain->extent = pCreateInfo->imageExtent;
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

    CtsSemaphoreCreateInfo semaphoreCreateInfo;
    CtsImageCreateInfo imageCreateInfo;
    imageCreateInfo.sType = CTS_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = NULL;
    imageCreateInfo.arrayLayers = pCreateInfo->imageArrayLayers;
    imageCreateInfo.extent.width = pCreateInfo->imageExtent.width;
    imageCreateInfo.extent.height = pCreateInfo->imageExtent.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.format = pCreateInfo->imageFormat;
    imageCreateInfo.imageType = CTS_IMAGE_TYPE_2D;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.queueFamilyIndexCount = pCreateInfo->queueFamilyIndexCount;
    imageCreateInfo.pQueueFamilyIndices = pCreateInfo->pQueueFamilyIndices;
    imageCreateInfo.samples = CTS_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.usage = pCreateInfo->imageUsage;

    for (uint32_t i = 0; i < pCreateInfo->minImageCount; ++i) {
        CtsSwapchainEntry* entry = &swapchain->pEntries[i];

        ctsCreateSemaphore(device, &semaphoreCreateInfo, pAllocator, &entry->semaphore);
        ctsCreateImageImpl(device, &imageCreateInfo, pAllocator, &entry->image);

        // Make sure the first wait never blocks
        ctsSignalSemaphores(1, &entry->semaphore);
    }

    ctsGetPhysicalDeviceSurfaceCapabilities(
        device->physicalDevice,
        device->physicalDevice->surface,
        &swapchain->surfaceCapabilities
    );

    ctsInitFSTextureHelper();
    *pSwapchain = swapchain;

    return CTS_SUCCESS;
}

void ctsDestroySwapchainImpl(
    CtsDevice device,
    CtsSwapchain swapchain,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;
    
    if (swapchain != NULL) {
        for (uint32_t i = 0; i < swapchain->entryCount; ++i) {
            CtsSwapchainEntry* entry = &swapchain->pEntries[i];

            if (entry->image != NULL) {
                ctsDestroyImageImpl(device, entry->image, pAllocator);
            }

            if (entry->semaphore != NULL) {
                ctsDestroySemaphore(device, entry->semaphore, pAllocator);
            }
        }

        ctsFree(pAllocator, swapchain->pEntries);
        ctsFree(pAllocator, swapchain);
        ctsCleanupFSTextureHelper();
    }
}

CtsResult ctsQueuePresentImpl(
    CtsQueue queue,
    const CtsPresentInfo* pPresentInfo
) {
    ctsWaitSemaphores(pPresentInfo->waitSemaphoreCount, pPresentInfo->pWaitSemaphores);

    for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i) {
        uint32_t imageIndex = pPresentInfo->pImageIndices[i];
        CtsSwapchain swapchain = pPresentInfo->pSwapchains[i];
        CtsDevice device = swapchain->device;
        CtsPhysicalDevice physicalDevice = swapchain->device->physicalDevice;
        CtsSwapchainEntry* entry = &swapchain->pEntries[imageIndex];

        CtsResult result = validateSwapchainSurface(swapchain);
        
        if (result != CTS_SUCCESS) {
            ctsSignalSemaphores(1, &entry->semaphore);
            return result;
        }

        ctsDrawFSTexture(device, entry->image);
        ctsSurfaceSwapBuffers(physicalDevice->surface);
        ctsSignalSemaphores(1, &entry->semaphore);
    }

    return CTS_SUCCESS;
}

CtsResult validateSwapchainSurface(CtsSwapchain swapchain) {
    CtsSurface surface = swapchain->device->physicalDevice->surface;
    CtsExtent2D* swapChainExtent = &swapchain->extent;
    CtsExtent2D* surfaceMinExtent = &swapchain->surfaceCapabilities.minImageExtent;
    CtsExtent2D* surfaceMaxExtent = &swapchain->surfaceCapabilities.maxImageExtent;

    CtsExtent2D surfaceExtent;
    if (ctsGetSurfaceExtent(surface, &surfaceExtent) == CTS_SUCCESS) {
        if (surfaceExtent.width < surfaceMinExtent->width   || 
            surfaceExtent.width > surfaceMaxExtent->width   || 
            surfaceExtent.height < surfaceMinExtent->height || 
            surfaceExtent.height > surfaceMaxExtent->height
        ) {
            return CTS_ERROR_OUT_OF_DATE;
        }

        if (surfaceExtent.width != swapChainExtent->width ||
            surfaceExtent.height != swapChainExtent->height
        ) {
            return CTS_SUBOPTIMAL;
        }

        return CTS_SUCCESS;
    }

    return CTS_ERROR_UNKNOWN;
}

#ifdef __cplusplus
}
#endif