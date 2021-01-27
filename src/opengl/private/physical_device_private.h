#pragma once

#include <cts/typedefs/instance.h>
#include <cts/typedefs/surface.h>

#ifdef __cplusplus
extern "C" {
#endif

static const CtsQueueFamilyProperties gQueueFamilyProperties = {
    .queueFlags = CTS_QUEUE_GRAPHICS_BIT | CTS_QUEUE_COMPUTE_BIT | CTS_QUEUE_TRANSFER_BIT,
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

static CtsPhysicalDeviceFeatures gPhysicalDeviceFeatures;
static CtsPhysicalDeviceProperties gPhysicalDeviceProperties;

static const uint32_t HEAP_BITS_BUFFER = ((1 << HEAP_INDEX_BUFFERS_HOST) | (1 << HEAP_INDEX_BUFFERS_DEVICE));
static const uint32_t HEAP_BITS_IMAGE  = ((1 << HEAP_INDEX_IMAGES_HOST)  | (1 << HEAP_INDEX_IMAGES_DEVICE));

static const CtsPhysicalDeviceMemoryProperties gPhysicalMemoryProperties = {
    .memoryTypeCount = 4,
    .memoryTypes = {
        {
            .propertyFlags = CTS_MEMORY_PROPERTY_HOST_VISIBLE_BIT | CTS_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            .heapIndex = HEAP_INDEX_BUFFERS_HOST
        }, {
            .propertyFlags = CTS_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .heapIndex = HEAP_INDEX_BUFFERS_DEVICE
        }, {
            .propertyFlags = CTS_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            .heapIndex = HEAP_INDEX_IMAGES_HOST
        }, {
            .propertyFlags = CTS_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
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
            .flags = CTS_MEMORY_HEAP_DEVICE_LOCAL_BIT
        }, {
            // HEAP_INDEX_IMAGES_HOST
            .size = UINT32_MAX,
            .flags = 0
        }, {
            // HEAP_INDEX_IMAGES_DEVICE
            .size = UINT32_MAX,
            .flags = CTS_MEMORY_HEAP_DEVICE_LOCAL_BIT
        }
    }
};

struct CtsPhysicalDeviceImpl {
    CtsInstance instance;
    CtsSurface surface;

    CtsMutex mutex;
    CtsConditionVariable conditionVariable;

    bool isRunning;
    bool isInitialized;
    CtsQueue queue;
};

void ctsPhysicalDeviceInit(CtsPhysicalDevice physicalDevice, CtsInstance instance, const CtsAllocationCallbacks* pAllocator);
void ctsPhysicalDeviceDestroy(CtsPhysicalDevice physicalDevice, const CtsAllocationCallbacks* pAllocator);

void ctsPhysicalDeviceSetSurface(CtsPhysicalDevice physicalDevice, CtsSurface surface);
bool ctsPhysicalDeviceWaitSurface(CtsPhysicalDevice physicalDevice);

void ctsPhysicalDeviceParseFeatures(CtsPhysicalDevice physicalDevice);
const CtsMemoryType* ctsGetMemoryType(uint32_t memoryTypeIndex);

#ifdef __cplusplus
}
#endif
