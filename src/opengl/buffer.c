#include <stddef.h>
#include <cts/buffer.h>
#include <cts/type_mapper.h>
#include <private/buffer_private.h>
#include <private/device_memory_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateBuffer(
    CtsDevice device,
    const CtsBufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsBuffer* pBuffer
) {
    (void) device;
    
    CtsBuffer buffer = ctsAllocation(
        pAllocator,
        sizeof(struct CtsBuffer),
        alignof(struct CtsBuffer),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (buffer == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }
        
    //GLenum usage = mapBufferUsage(pCreateInfo->usage);
    buffer->memory = NULL;
    buffer->type = parseBufferType(pCreateInfo->type);
    buffer->size = (GLsizei) pCreateInfo->size;
    buffer->offset = 0;

    *pBuffer = buffer;
    return CTS_SUCCESS;
}

CtsResult ctsBindBufferMemory(
    CtsDevice device,
    CtsBuffer buffer, 
    CtsDeviceMemory memory,
    CtsDeviceSize offset
) {
    if (buffer->size > memory->size - offset) {
        return CTS_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    buffer->memory = memory;
    buffer->offset = (GLsizei)offset;
    return CTS_SUCCESS;
}

void ctsDestroyBuffer(
    CtsDevice device,
    CtsBuffer buffer,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) device;

    if (buffer != NULL) {
        ctsFree(pAllocator, buffer);
    }
}

#ifdef __cplusplus
}
#endif