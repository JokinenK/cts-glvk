#include <stddef.h>
#include <cts/device_memory.h>
#include <cts/commands.h>
#include <private/device_memory_private.h>
#include <private/device_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsAllocateMemory(
    CtsDevice pDevice,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
) {
    CtsResult result;
    CtsAllocateMemory cmd;
    cmd.base.type = CTS_COMMAND_ALLOCATE_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.allocateInfo = pAllocateInfo;
    cmd.allocator = pAllocator;
    cmd.memory = pMemory;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

CtsResult ctsMapMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset,
    CtsDeviceSize pSize,
    CtsMemoryMapFlags pFlags,
    void** pData
) {
    CtsResult result;
    CtsMapMemory cmd;
    cmd.base.type = CTS_COMMAND_MAP_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.memory = pMemory;
    cmd.offset = pOffset;
    cmd.size = pSize;
    cmd.flags = pFlags;
    cmd.data = pData;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

void ctsUnmapMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory
) {
    CtsMapMemory cmd;
    cmd.base.type = CTS_COMMAND_UNMAP_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.memory = pMemory;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);
}

void ctsFreeMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsFreeMemory cmd;
    cmd.base.type = CTS_COMMAND_FREE_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.memory = pMemory;
    cmd.allocator = pAllocator;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);
}

CtsResult ctsAllocateMemoryImpl(
    CtsDevice pDevice,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
) {
    (void) pDevice;
    
    CtsDeviceMemory memory = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDeviceMemory),
        alignof(struct CtsDeviceMemory),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    ); 

    if (memory == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    memory->size = pAllocateInfo->allocationSize;

    glGenBuffers(1, &memory->handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
    glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)(pAllocateInfo->allocationSize), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    *pMemory = memory;
    return CTS_SUCCESS;
}

CtsResult ctsMapMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset,
    CtsDeviceSize pSize,
    CtsMemoryMapFlags pFlags,
    void** pData
) {
    if (pMemory == NULL) {
        return CTS_ERROR_MEMORY_MAP_FAILED;
    }

    (void) pFlags;
    GLuint flags = GL_MAP_READ_BIT  | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glBindBuffer(GL_COPY_WRITE_BUFFER, pMemory->handle);
    void* data = glMapBufferRange(GL_COPY_WRITE_BUFFER, pOffset, pSize, flags);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    if (data == NULL) {
        return CTS_ERROR_MEMORY_MAP_FAILED;
    }

    *pData = data;
    return CTS_SUCCESS;
}

void ctsUnmapMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory
) {
    if (pMemory == NULL) {
        return;
    }

    glBindBuffer(GL_COPY_WRITE_BUFFER, pMemory->handle);
    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void ctsFreeMemoryImpl(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pMemory != NULL) {
        glDeleteBuffers(1, &pMemory->handle);
        ctsFree(pAllocator, pMemory);
    }
}

#ifdef __cplusplus
}
#endif