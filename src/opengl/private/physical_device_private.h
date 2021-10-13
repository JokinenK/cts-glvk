#pragma once

#include "cts/allocator.h"
#include "vulkan/vulkan_core.h"
#include "cts/platform/platform_mutex.h"
#include "cts/platform/platform_condition_variable.h"

#ifdef __cplusplus
extern "C" {
#endif

static const VkQueueFamilyProperties gQueueFamilyProperties = {
    .queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT,
    .queueCount = 1,
    .timestampValidBits = 1,
    .minImageTransferGranularity = {0, 0, 0}
};

enum {
    HEAP_INDEX_BUFFERS_HOST   = 0,
    HEAP_INDEX_BUFFERS_DEVICE = 1,
    HEAP_INDEX_IMAGES_HOST    = 2,
    HEAP_INDEX_IMAGES_DEVICE  = 3
};

static VkPhysicalDeviceFeatures gPhysicalDeviceFeatures;
static VkPhysicalDeviceProperties gPhysicalDeviceProperties;

static const uint32_t HEAP_BITS_BUFFER = ((1 << HEAP_INDEX_BUFFERS_HOST) | (1 << HEAP_INDEX_BUFFERS_DEVICE));
static const uint32_t HEAP_BITS_IMAGE  = ((1 << HEAP_INDEX_IMAGES_HOST)  | (1 << HEAP_INDEX_IMAGES_DEVICE));

static const VkPhysicalDeviceMemoryProperties gPhysicalMemoryProperties = {
    .memoryTypeCount = 4,
    .memoryTypes = {
        {
            .propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            .heapIndex = HEAP_INDEX_BUFFERS_HOST
        }, {
            .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .heapIndex = HEAP_INDEX_BUFFERS_DEVICE
        }, {
            .propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            .heapIndex = HEAP_INDEX_IMAGES_HOST
        }, {
            .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .heapIndex = HEAP_INDEX_IMAGES_DEVICE
        }
    },
    .memoryHeapCount = 4,
    .memoryHeaps = {
        {
            // HEAP_INDEX_BUFFERS_HOST
            .size = UINT32_MAX,
            .flags = 0
        }, {
            // HEAP_INDEX_BUFFERS_DEVICE
            .size = UINT32_MAX,
            .flags = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT
        }, {
            // HEAP_INDEX_IMAGES_HOST
            .size = UINT32_MAX,
            .flags = 0
        }, {
            // HEAP_INDEX_IMAGES_DEVICE
            .size = UINT32_MAX,
            .flags = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT
        }
    }
};

struct CtsPhysicalDevice {
    struct CtsInstance* instance;
    struct CtsSurface* surface;

    CtsPlatformMutex mutex;
    CtsPlatformConditionVariable conditionVariable;

    bool isRunning;
    bool isInitialized;
    struct CtsQueue* queue;
};

void ctsPhysicalDeviceInit(struct CtsPhysicalDevice* physicalDevice, struct CtsInstance* instance, const VkAllocationCallbacks* pAllocator);
void ctsPhysicalDeviceDestroy(struct CtsPhysicalDevice* physicalDevice, const VkAllocationCallbacks* pAllocator);

void ctsPhysicalDeviceSetSurface(struct CtsPhysicalDevice* physicalDevice, struct CtsSurface* surface);
bool ctsPhysicalDeviceWaitSurface(struct CtsPhysicalDevice* physicalDevice);

void ctsPhysicalDeviceParseFeatures(struct CtsPhysicalDevice* physicalDevice);
const VkMemoryType* ctsGetMemoryType(uint32_t memoryTypeIndex);

#ifdef __cplusplus
}
#endif
