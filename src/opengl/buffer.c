#include <assert.h>
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
        sizeof(struct CtsBufferImpl),
        alignof(struct CtsBufferImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (buffer == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }
        
    buffer->memory = NULL;
    buffer->type = parseBufferUsage(pCreateInfo->usage);
    buffer->size = (GLsizei) pCreateInfo->size;
    buffer->offset = 0;
    buffer->usage = pCreateInfo->usage;

    *pBuffer = buffer;
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