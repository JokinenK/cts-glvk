#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include "cts/constants.h"
#include "cts/physical_device.h"
#include "cts/device_memory.h"
#include "cts/commands.h"
#include "cts/type_mapper.h"
#include "cts/private.h"
#include "cts/device_memory_private.h"
#include "cts/device_private.h"
#include "cts/buffer_private.h"
#include "cts/image_private.h"
#include "cts/queue_private.h"
#include "cts/physical_device_private.h"

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

VkResult VKAPI_CALL ctsAllocateMemory(
    VkDevice deviceHandle,
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsAllocateMemory cmd;
    cmd.base.pNext = NULL;
    cmd.base.pMetadata = CTS_COMMAND_METADATA(CtsAllocateMemory);

    cmd.device = deviceHandle;
    cmd.pAllocateInfo = pAllocateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pMemory = pMemory;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

VkResult VKAPI_CALL ctsMapMemory(
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
    cmd.base.pNext = NULL;
    cmd.base.pMetadata = CTS_COMMAND_METADATA(CtsMapMemory);

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

void VKAPI_CALL ctsUnmapMemory(
    VkDevice deviceHandle,
    VkDeviceMemory memory
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsUnmapMemory cmd;
    cmd.base.pNext = NULL;
    cmd.base.pMetadata = CTS_COMMAND_METADATA(CtsUnmapMemory);

    cmd.device = deviceHandle;
    cmd.memory = memory;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult VKAPI_CALL ctsFlushMappedMemoryRanges(
    VkDevice deviceHandle,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsFlushMappedMemoryRanges cmd;
    cmd.base.pNext = NULL;
    cmd.base.pMetadata = CTS_COMMAND_METADATA(CtsFlushMappedMemoryRanges);

    cmd.device = deviceHandle;
    cmd.memoryRangeCount = memoryRangeCount;
    cmd.pMemoryRanges = pMemoryRanges;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

void VKAPI_CALL ctsFreeMemory(
    VkDevice deviceHandle,
    VkDeviceMemory memory,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsFreeMemory cmd;
    cmd.base.pNext = NULL;
    cmd.base.pMetadata = CTS_COMMAND_METADATA(CtsFreeMemory);

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

    set_loader_magic_value(memory);

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
    GLuint actualSize = (size == VK_WHOLE_SIZE)
        ? memory->size
        : size;

    if (isBufferMemory(memory)) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
        memory->pMappedMemory = glMapBufferRange(GL_COPY_WRITE_BUFFER, offset, actualSize, mapFlags);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    } else if (isMappableImage(memory)) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, memory->handle);
        readImages(memory);
        memory->pMappedMemory = glMapBufferRange(GL_PIXEL_PACK_BUFFER, offset, actualSize, mapFlags);
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

VkResult VKAPI_CALL ctsBindBufferMemory(
    VkDevice device,
    VkBuffer buffer, 
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset
) {
    VkBindBufferMemoryInfo bindBufferMemoryInfo;
    bindBufferMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO;
    bindBufferMemoryInfo.pNext = NULL;
    bindBufferMemoryInfo.buffer = buffer;
    bindBufferMemoryInfo.memory = memory;
    bindBufferMemoryInfo.memoryOffset = memoryOffset;

    return ctsBindBufferMemory2(device, 1, &bindBufferMemoryInfo);
}

VkResult VKAPI_CALL ctsBindBufferMemory2(
    VkDevice deviceHandle,
    uint32_t bindInfoCount,
    const VkBindBufferMemoryInfo* pBindInfos
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    for (uint32_t i = 0; i < bindInfoCount; ++i) {
        const VkBindBufferMemoryInfo* pBindInfo = &pBindInfos[i];
        struct CtsBuffer* buffer = CtsBufferFromHandle(pBindInfo->buffer);
        struct CtsDeviceMemory* memory = CtsDeviceMemoryFromHandle(pBindInfo->memory);

        if (!isBufferMemory(memory)) {
            return VK_ERROR_FORMAT_NOT_SUPPORTED;
        }

        if (buffer->memory == NULL) {
            if (buffer->size > memory->size - pBindInfo->memoryOffset) {
                return VK_ERROR_OUT_OF_DEVICE_MEMORY;
            }

            buffer->memory = memory;
            buffer->offset = (GLsizei)pBindInfo->memoryOffset;
        }
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsBindImageMemory(
    VkDevice device,
    VkImage image,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset
) {
    VkBindImageMemoryInfo bindImageMemoryInfo;
    bindImageMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
    bindImageMemoryInfo.pNext = NULL;
    bindImageMemoryInfo.image = image;
    bindImageMemoryInfo.memory = memory;
    bindImageMemoryInfo.memoryOffset = memoryOffset;

    return ctsBindImageMemory2(device, 1, &bindImageMemoryInfo);
}

VkResult ctsBindImageMemory2(
    VkDevice deviceHandle,
    uint32_t bindInfoCount,
    const VkBindImageMemoryInfo* pBindInfos
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    for (uint32_t i = 0; i < bindInfoCount; ++i) {
        const VkBindImageMemoryInfo* pBindInfo = &pBindInfos[i];
        struct CtsImage* image = CtsImageFromHandle(pBindInfo->image);
        struct CtsDeviceMemory* memory = CtsDeviceMemoryFromHandle(pBindInfo->memory);

        if (!isImageMemory(memory)) {
            return VK_ERROR_FORMAT_NOT_SUPPORTED;
        }

        if (image->memory == NULL) {
            if (image->size > memory->size - pBindInfo->memoryOffset) {
                return VK_ERROR_OUT_OF_DEVICE_MEMORY;
            }

            image->memory = memory;
            image->offset = pBindInfo->memoryOffset;
            appendImage(memory, image);
        }
    }

    return VK_SUCCESS;
}


void VKAPI_CALL ctsGetBufferMemoryRequirements(
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

        default: {
            align = 256;
        }
    }

    pMemoryRequirements->size = buffer->size;
    pMemoryRequirements->alignment = align;
    pMemoryRequirements->memoryTypeBits = HEAP_BITS_BUFFER;
}

void VKAPI_CALL ctsGetBufferMemoryRequirements2(
    VkDevice device,
    const VkBufferMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements
) {
    ctsGetBufferMemoryRequirements(device, pInfo->buffer, &pMemoryRequirements->memoryRequirements);
}

void VKAPI_CALL ctsGetImageMemoryRequirements(
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

void VKAPI_CALL ctsGetImageMemoryRequirements2(
    VkDevice device,
    const VkImageMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements
) {
    ctsGetImageMemoryRequirements(device, pInfo->image, &pMemoryRequirements->memoryRequirements);
}


void VKAPI_CALL ctsGetImageSparseMemoryRequirements(
    VkDevice device,
    VkImage image,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements* pSparseMemoryRequirements
) {
    fprintf(stderr, "ctsGetImageSparseMemoryRequirements - not implemented");
    abort();
}

void VKAPI_CALL vkGetImageSparseMemoryRequirements2(
    VkDevice device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements
) {
    if (pSparseMemoryRequirementCount != NULL) {
        ctsGetImageSparseMemoryRequirements(device, pInfo->image, pSparseMemoryRequirementCount, NULL);

        for (uint32_t i = 0; i < *pSparseMemoryRequirementCount; ++i) {
            pSparseMemoryRequirements[i].sType = VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2;
            pSparseMemoryRequirements[i].pNext = NULL; 

            uint32_t one = 1;
            ctsGetImageSparseMemoryRequirements(device, pInfo->image, &one, &pSparseMemoryRequirements[i].memoryRequirements);
        }
    }
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