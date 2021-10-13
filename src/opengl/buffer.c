#include <assert.h>
#include <stddef.h>
#include "vulkan/vulkan_core.h"
#include "cts/allocator.h"
#include "cts/buffer.h"
#include "cts/util/align.h"
#include <cts/type_mapper.h>
#include <private/private.h>
#include <private/buffer_private.h>
#include <private/device_memory_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult ctsCreateBuffer(
    VkDevice deviceHandle,
    const VkBufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer* pBuffer
) {
    (void) deviceHandle;
    
    struct CtsBuffer* buffer = ctsAllocation(
        pAllocator,
        sizeof(struct CtsBuffer),
        alignof(struct CtsBuffer),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (buffer == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
        
    buffer->memory = NULL;
    buffer->type = parseBufferUsage(pCreateInfo->usage);
    buffer->size = (GLsizei) pCreateInfo->size;
    buffer->offset = 0;
    buffer->usage = pCreateInfo->usage;

    *pBuffer = CtsBufferToHandle(buffer);
    return VK_SUCCESS;
}

void ctsDestroyBuffer(
    VkDevice deviceHandle,
    VkBuffer bufferHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;
    struct CtsBuffer* buffer = CtsBufferFromHandle(bufferHandle);

    if (buffer != NULL) {
        ctsFree(pAllocator, buffer);
    }
}

#ifdef __cplusplus
}
#endif