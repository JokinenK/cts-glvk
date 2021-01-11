#include <stddef.h>
#include <cts/buffer.h>
#include <cts/type_mapper.h>
#include <private/buffer_private.h>
#include <private/device_memory_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateBuffer(
    CtsDevice pDevice,
    const CtsBufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsBuffer* pBuffer
) {
    (void) pDevice;
    
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
    CtsDevice pDevice,
    CtsBuffer pBuffer, 
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset
) {
    if (pBuffer->size > pMemory->size - pOffset) {
        return CTS_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    pBuffer->memory = pMemory;
    pBuffer->offset = (GLsizei)pOffset;
    return CTS_SUCCESS;
}

void ctsDestroyBuffer(
    CtsDevice pDevice,
    CtsBuffer pBuffer,
    const CtsAllocationCallbacks* pAllocator
) {
    (void) pDevice;

    if (pBuffer != NULL) {
        ctsFree(pAllocator, pBuffer);
    }
}

#ifdef __cplusplus
}
#endif