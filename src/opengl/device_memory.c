#include <assert.h>
#include <stddef.h>
#include <cts/constants.h>
#include <cts/physical_device.h>
#include <cts/device_memory.h>
#include <cts/commands.h>
#include <cts/type_mapper.h>
#include <private/device_memory_private.h>
#include <private/device_private.h>
#include <private/buffer_private.h>
#include <private/image_private.h>
#include <private/queue_private.h>
#include <private/physical_device_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static bool isHostVisible(CtsDeviceMemory memory);
static bool isBufferMemory(CtsDeviceMemory memory);
static bool isImageMemory(CtsDeviceMemory memory);
static bool isMappableImage(CtsDeviceMemory memory);

static void flushMemoryRange(CtsDeviceMemory memory, size_t offset, size_t size);
static GLuint createBufferMapFlags(CtsMemoryPropertyFlags propertyFlags);
static CtsDeviceSize multipleOf(CtsDeviceSize value, CtsDeviceSize multiple);

static void appendImage(CtsDeviceMemory deviceMemory, CtsImage image);

static void readImages(CtsDeviceMemory deviceMemory);
static void writeImages(CtsDeviceMemory deviceMemory);

CtsResult ctsAllocateMemory(
    CtsDevice device,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
) {
    CtsResult result;
    CtsAllocateMemory cmd;
    cmd.base.type = CTS_COMMAND_ALLOCATE_MEMORY;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pAllocateInfo = pAllocateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pMemory = pMemory;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

CtsResult ctsMapMemory(
    CtsDevice device,
    CtsDeviceMemory memory,
    CtsDeviceSize offset,
    CtsDeviceSize size,
    CtsMemoryMapFlags flags,
    void** ppData
) {
    CtsResult result;
    CtsMapMemory cmd;
    cmd.base.type = CTS_COMMAND_MAP_MEMORY;
    cmd.base.pNext = NULL;

    cmd.device = device;
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
    CtsDevice device,
    CtsDeviceMemory memory
) {
    CtsMapMemory cmd;
    cmd.base.type = CTS_COMMAND_UNMAP_MEMORY;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.memory = memory;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsFlushMappedMemoryRanges(
    CtsDevice device,
    uint32_t memoryRangeCount,
    const CtsMappedMemoryRange* pMemoryRanges
) {
    CtsResult result;
    CtsFlushMappedMemoryRanges cmd;
    cmd.base.type = CTS_COMMAND_FLUSH_MAPPED_MEMORY_RANGES;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.memoryRangeCount = memoryRangeCount;
    cmd.pMemoryRanges = pMemoryRanges;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

void ctsFreeMemory(
    CtsDevice device,
    CtsDeviceMemory memory,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsFreeMemory cmd;
    cmd.base.type = CTS_COMMAND_FREE_MEMORY;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.memory = memory;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsAllocateMemoryImpl(
    CtsDevice device,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
) {
    (void) device;
    
    CtsDeviceMemory memory = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDeviceMemoryImpl),
        alignof(struct CtsDeviceMemoryImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (memory == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    const CtsMemoryType* pMemoryType = ctsGetMemoryType(pAllocateInfo->memoryTypeIndex);
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

    *pMemory = memory;
    return CTS_SUCCESS;
}

CtsResult ctsMapMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory,
    CtsDeviceSize offset,
    CtsDeviceSize size,
    CtsMemoryMapFlags flags,
    void** ppData
) {
    (void) flags;

    if (!isHostVisible(memory)) {
        return CTS_ERROR_MEMORY_MAP_FAILED;
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
        return CTS_ERROR_MEMORY_MAP_FAILED;
    }

    *ppData = memory->pMappedMemory;
    return CTS_SUCCESS;
}

void ctsUnmapMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory
) {
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
        
        if (memory->propertyFlags & CTS_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
            flushMemoryRange(memory, 0, memory->size);
        }
    } else {
        /* Unmappable image, do nothing */
    }

    memory->pMappedMemory = NULL;
}

CtsResult ctsFlushMappedMemoryRangesImpl(
    CtsDevice device,
    uint32_t memoryRangeCount,
    const CtsMappedMemoryRange* pMemoryRanges
) {
    for (uint32_t i = 0; i < memoryRangeCount; ++i) {
        const CtsMappedMemoryRange* pMemoryRange = &pMemoryRanges[i];
        flushMemoryRange(pMemoryRange->memory, pMemoryRange->offset, pMemoryRange->size);
    }

    return CTS_SUCCESS;
}

void ctsFreeMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory,
    const CtsAllocationCallbacks* pAllocator
) {
    if (memory != NULL) {
        ctsUnmapMemoryImpl(device, memory);

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

CtsResult ctsBindBufferMemory(
    CtsDevice device,
    CtsBuffer buffer, 
    CtsDeviceMemory memory,
    CtsDeviceSize memoryOffset
) {
    if (!isBufferMemory(memory)) {
        return CTS_ERROR_FORMAT_NOT_SUPPORTED;
    }

    if (buffer->memory == NULL) {
        if (buffer->size > memory->size - memoryOffset) {
            return CTS_ERROR_OUT_OF_DEVICE_MEMORY;
        }

        buffer->memory = memory;
        buffer->offset = (GLsizei)memoryOffset;
    }

    return CTS_SUCCESS;
}

CtsResult ctsBindImageMemory(
    CtsDevice device,
    CtsImage image,
    CtsDeviceMemory memory,
    CtsDeviceSize memoryOffset
) {
    if (!isImageMemory(memory)) {
        return CTS_ERROR_FORMAT_NOT_SUPPORTED;
    }

    if (image->memory == NULL) {
        if (image->size > memory->size - memoryOffset) {
            return CTS_ERROR_OUT_OF_DEVICE_MEMORY;
        }

        image->memory = memory;
        image->offset = memoryOffset;
        appendImage(memory, image);
    }
    
    return CTS_SUCCESS;
}

void ctsGetBufferMemoryRequirements(
    CtsDevice device,
    CtsBuffer buffer,
    CtsMemoryRequirements* pMemoryRequirements
) {
    CtsDeviceSize align = 0;
    GLenum bufferType = parseBufferUsage(buffer->usage);

    switch (bufferType) {
        case GL_ARRAY_BUFFER:
        case GL_ELEMENT_ARRAY_BUFFER: {
            align = 8;
        } break;

        case GL_UNIFORM_BUFFER: {
            align = getUniformBufferOffsetAlignment();
        } break;

        case GL_TEXTURE_BUFFER: {
            align = getTextureBufferOffsetAlignment();
        } break;
    }

    pMemoryRequirements->size = buffer->size;
    pMemoryRequirements->alignment = align;
    pMemoryRequirements->memoryTypeBits = HEAP_BITS_BUFFER;
}

void ctsGetImageMemoryRequirements(
    CtsDevice device,
    CtsImage image,
    CtsMemoryRequirements* pMemoryRequirements
) {
    pMemoryRequirements->size = image->size;
    pMemoryRequirements->alignment = 1;
    pMemoryRequirements->memoryTypeBits = HEAP_BITS_IMAGE;
}

static bool isHostVisible(CtsDeviceMemory memory) {
    return (memory->propertyFlags & CTS_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

static bool isBufferMemory(CtsDeviceMemory memory) {
    return (HEAP_BITS_BUFFER & (1 << memory->heapIndex));
}

static bool isImageMemory(CtsDeviceMemory memory) {
    return (HEAP_BITS_IMAGE & (1 << memory->heapIndex));
}

static bool isMappableImage(CtsDeviceMemory memory) {
    return isImageMemory(memory) && isHostVisible(memory);
}

static void flushMemoryRange(CtsDeviceMemory memory, size_t offset, size_t size) {
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

static GLuint createBufferMapFlags(CtsMemoryPropertyFlags propertyFlags) {
    GLuint flags = (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    if (propertyFlags & CTS_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        flags |= (GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT);
    }

    return flags;
}

static CtsDeviceSize multipleOf(CtsDeviceSize value, CtsDeviceSize multiple) {
    assert(multiple);
    return ((value + multiple - 1) / multiple) * multiple;
}

static void appendImage(CtsDeviceMemory deviceMemory, CtsImage image) {
    if (deviceMemory->images != NULL) {
        /*CtsImage lastImage = NULL;

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

static void readImages(CtsDeviceMemory deviceMemory) {
    CtsImage image = deviceMemory->images;
    uintptr_t offset = 0;

    for (; image != NULL; image = image->nextSibling) {
        glBindTexture(image->target, image->handle);
        offset += ctsImageToBuffer(image, offset);
    }
}

static void writeImages(CtsDeviceMemory deviceMemory) {
    CtsImage image = deviceMemory->images;
    uintptr_t offset = 0;

    for (; image != NULL; image = image->nextSibling) {
        glBindTexture(image->target, image->handle);
        offset += ctsBufferToImage(image, offset);
    }
}

#ifdef __cplusplus
}
#endif