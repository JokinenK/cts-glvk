#include <stdint.h>
#include <string.h>
#include "cts/swapchain.h"
#include "cts/semaphore.h"
#include "cts/gl_helper.h"
#include "cts/commands.h"
#include "cts/private.h"
#include "cts/device_private.h"
#include "cts/fence_private.h"
#include "cts/image_private.h"
#include "cts/physical_device_private.h"
#include "cts/swapchain_private.h"
#include "cts/semaphore_private.h"
#include "cts/surface_private.h"
#include "cts/queue_private.h"

#ifdef __cplusplus
extern "C" {
#endif

static bool gFSTextureInitialized = false;
static VkResult validateSwapchainSurface(struct CtsSwapchain* swapchain);

VkResult VKAPI_CALL ctsCreateSwapchainKHR(
    VkDevice deviceHandle,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsCreateSwapchain cmd;
    cmd.base.type = CTS_COMMAND_CREATE_SWAPCHAIN;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pSwapchain = pSwapchain;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void VKAPI_CALL ctsDestroySwapchainKHR(
    VkDevice deviceHandle,
    VkSwapchainKHR swapchain,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsDestroySwapchain cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_SWAPCHAIN;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.swapchain = swapchain;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult VKAPI_CALL ctsQueuePresentKHR(
    VkQueue queueHandle,
    const VkPresentInfoKHR* pPresentInfo
) {
    struct CtsQueue* queue = CtsQueueFromHandle(queueHandle);

    VkResult result;
    CtsQueuePresent cmd;
    cmd.base.type = CTS_COMMAND_QUEUE_PRESENT;
    cmd.base.pNext = NULL;

    cmd.queue = queueHandle;
    cmd.pPresentInfo = pPresentInfo;
    cmd.pResult = &result;

    ctsQueueDispatch(queue, &cmd.base);
    return result;
}

VkResult VKAPI_CALL ctsGetSwapchainImagesKHR(
    VkDevice deviceHandle,
    VkSwapchainKHR swapchainHandle,
    uint32_t* pSwapchainImageCount,
    VkImage* pSwapchainImages
) {
    (void) deviceHandle;

    struct CtsSwapchain* swapchain = CtsSwapchainFromHandle(swapchainHandle);

    if (pSwapchainImageCount != NULL) {
        *pSwapchainImageCount = swapchain->entryCount;
    }

    if (pSwapchainImages != NULL) {
        for (uint32_t i = 0; i < swapchain->entryCount; ++i) {
            pSwapchainImages[i] = swapchain->pEntries[i].image;
        }
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsAcquireNextImageKHR(
    VkDevice deviceHandle,
    VkSwapchainKHR swapchainHandle,
    uint64_t timeout,
    VkSemaphore semaphore,
    VkFence fence,
    uint32_t* pImageIndex
) {
    struct CtsSwapchain* swapchain = CtsSwapchainFromHandle(swapchainHandle);

    uint32_t imageIndex = swapchain->nextEntry;
    swapchain->nextEntry = (swapchain->nextEntry + 1) % swapchain->entryCount;

    struct CtsSwapchainEntry* entry = &swapchain->pEntries[imageIndex];

    if (semaphore != NULL) {
        ctsSignalSemaphores(1, &semaphore);
    }

    if (fence != NULL) {
        ctsSignalFence(deviceHandle, fence);
    }

    *pImageIndex = imageIndex;

    if (ctsWaitSemaphore(entry->semaphore, timeout) == VK_TIMEOUT) {
        return VK_TIMEOUT;
    }
    
    return validateSwapchainSurface(swapchain);
}

VkResult ctsCreateSwapchainKHRImpl(
    VkDevice deviceHandle,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsSwapchain* swapchain = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSwapchain),
        alignof(struct CtsSwapchain),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (swapchain == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(swapchain);

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
        sizeof(struct CtsSwapchainEntry) * swapchain->entryCount,
        alignof(struct CtsSwapchainEntry),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (swapchain->pEntries == NULL) {
        ctsDestroySwapchainKHR(deviceHandle, CtsSwapchainToHandle(swapchain), pAllocator);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    VkSemaphoreCreateInfo semaphoreCreateInfo;
    VkImageCreateInfo imageCreateInfo;
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = NULL;
    imageCreateInfo.arrayLayers = pCreateInfo->imageArrayLayers;
    imageCreateInfo.extent.width = pCreateInfo->imageExtent.width;
    imageCreateInfo.extent.height = pCreateInfo->imageExtent.height;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.format = pCreateInfo->imageFormat;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.queueFamilyIndexCount = pCreateInfo->queueFamilyIndexCount;
    imageCreateInfo.pQueueFamilyIndices = pCreateInfo->pQueueFamilyIndices;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.usage = pCreateInfo->imageUsage;

    for (uint32_t i = 0; i < pCreateInfo->minImageCount; ++i) {
        struct CtsSwapchainEntry* entry = &swapchain->pEntries[i];

        ctsCreateSemaphore(deviceHandle, &semaphoreCreateInfo, pAllocator, &entry->semaphore);
        ctsCreateImageImpl(deviceHandle, &imageCreateInfo, pAllocator, &entry->image);

        // Make sure the first wait never blocks
        ctsSignalSemaphores(1, &entry->semaphore);
    }

    ctsGetPhysicalDeviceSurfaceCapabilitiesKHR(
        CtsPhysicalDeviceToHandle(device->physicalDevice),
        CtsSurfaceToHandle(device->physicalDevice->surface),
        &swapchain->surfaceCapabilities
    );

    *pSwapchain = CtsSwapchainToHandle(swapchain);

    return VK_SUCCESS;
}

void ctsDestroySwapchainKHRImpl(
    VkDevice deviceHandle,
    VkSwapchainKHR swapchainHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;

    struct CtsSwapchain* swapchain = CtsSwapchainFromHandle(swapchainHandle);
    
    if (swapchain != NULL) {
        for (uint32_t i = 0; i < swapchain->entryCount; ++i) {
            struct CtsSwapchainEntry* entry = &swapchain->pEntries[i];

            if (entry->image != NULL) {
                ctsDestroyImageImpl(deviceHandle, entry->image, pAllocator);
            }

            if (entry->semaphore != NULL) {
                ctsDestroySemaphore(deviceHandle, entry->semaphore, pAllocator);
            }
        }

        ctsFree(pAllocator, swapchain->pEntries);
        ctsFree(pAllocator, swapchain);
    }
}

VkResult VKAPI_CALL ctsQueuePresentKHRImpl(
    VkQueue queueHandle,
    const VkPresentInfoKHR* pPresentInfo
) {
    ctsWaitSemaphores(pPresentInfo->waitSemaphoreCount, pPresentInfo->pWaitSemaphores);

    for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i) {
        uint32_t imageIndex = pPresentInfo->pImageIndices[i];
        struct CtsSwapchain* swapchain = CtsSwapchainFromHandle(pPresentInfo->pSwapchains[i]);
        struct CtsDevice* device = swapchain->device;
        struct CtsPhysicalDevice* physicalDevice = swapchain->device->physicalDevice;
        struct CtsSwapchainEntry* entry = &swapchain->pEntries[imageIndex];

        VkResult result = validateSwapchainSurface(swapchain);
        
        if (result != VK_SUCCESS) {
            ctsSignalSemaphores(1, &entry->semaphore);
            return result;
        }

        struct CtsGlContext* context = ctsSurfaceGetGlContext(physicalDevice->surface);
        struct CtsImage* image = CtsImageFromHandle(entry->image);
        
        ctsGlHelperDrawFSTexture(&context->helper, device, image);
        ctsGlContextSwapBuffers(context);
        
        ctsSignalSemaphores(1, &entry->semaphore);
    }

    return VK_SUCCESS;
}

VkResult validateSwapchainSurface(struct CtsSwapchain* swapchain) {
    struct CtsSurface* surface = swapchain->device->physicalDevice->surface;
    VkExtent2D* swapChainExtent = &swapchain->extent;
    VkExtent2D* surfaceMinExtent = &swapchain->surfaceCapabilities.minImageExtent;
    VkExtent2D* surfaceMaxExtent = &swapchain->surfaceCapabilities.maxImageExtent;

    VkExtent2D surfaceExtent;
    if (ctsGetSurfaceExtent(surface, &surfaceExtent) == VK_SUCCESS) {
        if (surfaceExtent.width < surfaceMinExtent->width   || 
            surfaceExtent.width > surfaceMaxExtent->width   || 
            surfaceExtent.height < surfaceMinExtent->height || 
            surfaceExtent.height > surfaceMaxExtent->height
        ) {
            return VK_ERROR_OUT_OF_DATE_KHR;
        }

        if (surfaceExtent.width != swapChainExtent->width ||
            surfaceExtent.height != swapChainExtent->height
        ) {
            return VK_SUBOPTIMAL_KHR;
        }

        return VK_SUCCESS;
    }

    return VK_ERROR_INITIALIZATION_FAILED;
}

#ifdef __cplusplus
}
#endif