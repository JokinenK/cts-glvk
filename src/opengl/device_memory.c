#include <assert.h>
#include <stddef.h>
#include <cts/constants.h>
#include <cts/physical_device.h>
#include <cts/device_memory.h>
#include <cts/commands.h>
#include <cts/type_mapper.h>
#include <private/private.h>
#include <private/device_memory_private.h>
#include <private/device_private.h>
#include <private/buffer_private.h>
#include <private/image_private.h>
#include <private/queue_private.h>
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static bool isHostVisible(struct CtsDeviceMemory* memory);
static bool isBufferMemory(struct CtsDeviceMemory* memory);
static bool isImageMemory(struct CtsDeviceMemory* memory);
static bool isMappableImage(struct CtsDeviceMemory* memory);

static void flushMemoryRange(struct CtsDeviceMemory* memory, size_t offset, size_t size);
static GLuint createBufferMapFlags(VkMemoryPropertyFlags propertyFlags);
static VkDeviceSize multipleOf(VkDeviceSize value, VkDeviceSize multiple);

static void appendImage(struct CtsDeviceMemory* deviceMemory, struct CtsImage* image);
static void readImages(struct CtsDeviceMemory* deviceMemory);
static void writeImages(struct CtsDeviceMemory* deviceMemory);

VkResult ctsAllocateMemory(
    VkDevice deviceHandle,
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsAllocateMemory cmd;
    cmd.base.type = CTS_COMMAND_ALLOCATE_MEMORY;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.pAllocateInfo = pAllocateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pMemory = pMemory;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

VkResult ctsMapMemory(
    VkDevice deviceHandle,
    VkDeviceMemory memory,
    VkDeviceSize offset,
    VkDeviceSize size,
    VkMemoryMapFlags flags,
    void** ppData
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsMapMemory cmd;
    cmd.base.type = CTS_COMMAND_MAP_MEMORY;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.memory = memory;
    cmd.offset = offset;
    cmd.size = size;
    cmd.flags = flags;
    cmd.ppData = ppData;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

void ctsUnmapMemory(
    VkDevice deviceHandle,
    VkDeviceMemory memory
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsMapMemory cmd;
    cmd.base.type = CTS_COMMAND_UNMAP_MEMORY;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.memory = memory;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult ctsFlushMappedMemoryRanges(
    VkDevice deviceHandle,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsFlushMappedMemoryRanges cmd;
    cmd.base.type = CTS_COMMAND_FLUSH_MAPPED_MEMORY_RANGES;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.memoryRangeCount = memoryRangeCount;
    cmd.pMemoryRanges = pMemoryRanges;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

void ctsFreeMemory(
    VkDevice deviceHandle,
    VkDeviceMemory memory,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsFreeMemory cmd;
    cmd.base.type = CTS_COMMAND_FREE_MEMORY;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.memory = memory;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult ctsAllocateMemoryImpl(
    VkDevice deviceHandle,
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory
) {
    (void) deviceHandle;
    
    struct CtsDeviceMemory* memory = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDeviceMemory),
        alignof(struct CtsDeviceMemory),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (memory == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    const VkMemoryType* pMemoryType = ctsGetMemoryType(pAllocateInfo->memoryTypeIndex);
    GLuint mapFlags = createBufferMapFlags(pMemoryType->propertyFlags);
    
    memory->size = pAllocateInfo->allocationSize;
    memory->heapIndex = pMemoryType->heapIndex;
    memory->propertyFlags = pMemoryType->propertyFlags;
    memory->pAllocator = pAllocator;
    memory->images = NULL;
    memory->pMappedMemory = NULL;

    if (isBufferMemory(memory)) {
        glGenBuffers(1, &memory->handle);
        glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
        glBufferStorage(GL_COPY_WRITE_BUFFER, (GLsizeiptr) memory->size, NULL, mapFlags);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    } else if (isMappableImage(memory)) {
        glGenBuffers(1, &memory->handle);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, memory->handle);
        glBufferStorage(GL_PIXEL_PACK_BUFFER, (GLsizeiptr) memory->size, NULL, mapFlags);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    } else {
        /* Unmappable image, do nothing */
    }

    *pMemory = CtsDeviceMemoryToHandle(memory);
    return VK_SUCCESS;
}

VkResult ctsMapMemoryImpl(
    VkDevice deviceHandle,
    VkDeviceMemory memoryHandle,
    VkDeviceSize offset,
    VkDeviceSize size,
    VkMemoryMapFlags flags,
    void** ppData
) {
    (void) flags;

    struct CtsDeviceMemory* memory = CtsDeviceMemoryFromHandle(memoryHandle);

    if (!isHostVisible(memory)) {
        return VK_ERROR_MEMORY_MAP_FAILED;
    }

    GLuint mapFlags = createBufferMapFlags(memory->propertyFlags);

    if (isBufferMemory(memory)) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
        memory->pMappedMemory = glMapBufferRange(GL_COPY_WRITE_BUFFER, offset, size, mapFlags);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    } else if (isMappableImage(memory)) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, memory->handle);
        readImages(memory);
        memory->pMappedMemory = glMapBufferRange(GL_PIXEL_PACK_BUFFER, offset, size, mapFlags);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    } else {
        /* Unmappable image, do nothing */
    }

    if (memory->pMappedMemory == NULL) {
        return VK_ERROR_MEMORY_MAP_FAILED;
    }

    *ppData = memory->pMappedMemory;
    return VK_SUCCESS;
}

void ctsUnmapMemoryImpl(
    VkDevice deviceHandle,
    VkDeviceMemory memoryHandle
) {
    struct CtsDeviceMemory* memory = CtsDeviceMemoryFromHandle(memoryHandle);

    if (memory->pMappedMemory == NULL) {
        return;
    }

    if (isBufferMemory(memory)) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
        glUnmapBuffer(GL_COPY_WRITE_BUFFER);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    } else if (isMappableImage(memory)) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, memory->handle);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        flushMemoryRange(memory, 0, memory->size);
    } else {
        /* Unmappable image, do nothing */
    }

    memory->pMappedMemory = NULL;
}

VkResult ctsFlushMappedMemoryRangesImpl(
    VkDevice deviceHandle,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges
) {
    for (uint32_t i = 0; i < memoryRangeCount; ++i) {
        const VkMappedMemoryRange* pMemoryRange = &pMemoryRanges[i];
        struct CtsDeviceMemory* memory = CtsDeviceMemoryFromHandle(pMemoryRange->memory);
        flushMemoryRange(memory, pMemoryRange->offset, pMemoryRange->size);
    }

    return VK_SUCCESS;
}

void ctsFreeMemoryImpl(
    VkDevice deviceHandle,
    VkDeviceMemory memoryHandle,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsDeviceMemory* memory = CtsDeviceMemoryFromHandle(memoryHandle);

    if (memory != NULL) {
        ctsUnmapMemoryImpl(deviceHandle, memoryHandle);

        if (isBufferMemory(memory)) {
            glDeleteBuffers(1, &memory->handle);
        } else if (isMappableImage(memory)) {
            glDeleteBuffers(1, &memory->handle);
        } else {
            /* Unmappable image, do nothing */
        }

        ctsFree(pAllocator, memory);
    }
}

VkResult ctsBindBufferMemory(
    VkDevice deviceHandle,
    VkBuffer bufferHandle, 
    VkDeviceMemory memoryHandle,
    VkDeviceSize memoryOffset
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsBuffer* buffer = CtsBufferFromHandle(bufferHandle);
    struct CtsDeviceMemory* memory = CtsDeviceMemoryFromHandle(memoryHandle);

    if (!isBufferMemory(memory)) {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    if (buffer->memory == NULL) {
        if (buffer->size > memory->size - memoryOffset) {
            return VK_ERROR_OUT_OF_DEVICE_MEMORY;
        }

        buffer->memory = memory;
        buffer->offset = (GLsizei)memoryOffset;
    }

    return VK_SUCCESS;
}

VkResult ctsBindImageMemory(
    VkDevice deviceHandle,
    VkImage imageHandle,
    VkDeviceMemory memoryHandle,
    VkDeviceSize memoryOffset
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsImage* image = CtsImageFromHandle(imageHandle);
    struct CtsDeviceMemory* memory = CtsDeviceMemoryFromHandle(memoryHandle);

    if (!isImageMemory(memory)) {
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    if (image->memory == NULL) {
        if (image->size > memory->size - memoryOffset) {
            return VK_ERROR_OUT_OF_DEVICE_MEMORY;
        }

        image->memory = memory;
        image->offset = memoryOffset;
        appendImage(memory, image);
    }
    
    return VK_SUCCESS;
}

void ctsGetBufferMemoryRequirements(
    VkDevice deviceHandle,
    VkBuffer bufferHandle,
    VkMemoryRequirements* pMemoryRequirements
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsBuffer* buffer = CtsBufferFromHandle(bufferHandle);

    VkDeviceSize align = 0;
    GLenum bufferType = parseBufferUsage(buffer->usage);

    switch (bufferType) {
        case GL_ARRAY_BUFFER:
        case GL_ELEMENT_ARRAY_BUFFER: {
            align = 8;
        } break;

        case GL_UNIFORM_BUFFER: {
            align = gPhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
        } break;

        case GL_TEXTURE_BUFFER: {
            align = gPhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment;
        } break;
    }

    pMemoryRequirements->size = buffer->size;
    pMemoryRequirements->alignment = align;
    pMemoryRequirements->memoryTypeBits = HEAP_BITS_BUFFER;
}

void ctsGetImageMemoryRequirements(
    VkDevice deviceHandle,
    VkImage imageHandle,
    VkMemoryRequirements* pMemoryRequirements
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsImage* image = CtsImageFromHandle(imageHandle);

    pMemoryRequirements->size = image->size;
    pMemoryRequirements->alignment = 1;
    pMemoryRequirements->memoryTypeBits = HEAP_BITS_IMAGE;
}

static bool isHostVisible(struct CtsDeviceMemory* memory) {
    return (memory->propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

static bool isBufferMemory(struct CtsDeviceMemory* memory) {
    return (HEAP_BITS_BUFFER & (1 << memory->heapIndex));
}

static bool isImageMemory(struct CtsDeviceMemory* memory) {
    return (HEAP_BITS_IMAGE & (1 << memory->heapIndex));
}

static bool isMappableImage(struct CtsDeviceMemory* memory) {
    return isImageMemory(memory) && isHostVisible(memory);
}

static void flushMemoryRange(struct CtsDeviceMemory* memory, size_t offset, size_t size) {
    if (isBufferMemory(memory)) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
        glFlushMappedBufferRange(GL_COPY_WRITE_BUFFER, offset, size);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    } else if (isMappableImage(memory)) {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, memory->handle);
        writeImages(memory);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    } else {
        /* Unmappable image, do nothing */
    }
}

static GLuint createBufferMapFlags(VkMemoryPropertyFlags propertyFlags) {
    GLuint flags = (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    if (propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        flags |= (GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
    }

    return flags;
}

static VkDeviceSize multipleOf(VkDeviceSize value, VkDeviceSize multiple) {
    assert(multiple);
    return ((value + multiple - 1) / multiple) * multiple;
}

static void appendImage(struct CtsDeviceMemory* deviceMemory, struct CtsImage* image) {
    if (deviceMemory->images != NULL) {
        /*VkImage lastImage = NULL;

        for (
            lastImage = deviceMemory->images;
            lastImage->nextSibling != NULL;
            lastImage = lastImage->nextSibling
        );

        lastImage->nextSibling = image;*/
    } else {
        deviceMemory->images = image;
    }
}

static void readImages(struct CtsDeviceMemory* deviceMemory) {
    struct CtsImage* image = deviceMemory->images;
    uintptr_t offset = 0;

    for (; image != NULL; image = image->nextSibling) {
        glBindTexture(image->target, image->handle);
        offset += ctsImageToBuffer(image, offset);
    }
}

static void writeImages(struct CtsDeviceMemory* deviceMemory) {
    struct CtsImage* image = deviceMemory->images;
    uintptr_t offset = 0;

    for (; image != NULL; image = image->nextSibling) {
        glBindTexture(image->target, image->handle);
        offset += ctsBufferToImage(image, offset);
    }
}

#ifdef __cplusplus
}
#endif