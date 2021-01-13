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

    ctsQueueDispatch(device->queue, &cmd.base, device->dispatchSemaphore);
    ctsWaitSemaphores(1, &device->dispatchSemaphore);

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

    ctsQueueDispatch(device->queue, &cmd.base, device->dispatchSemaphore);
    ctsWaitSemaphores(1, &device->dispatchSemaphore);

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

    ctsQueueDispatch(device->queue, &cmd.base, device->dispatchSemaphore);
    ctsWaitSemaphores(1, &device->dispatchSemaphore);
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

    ctsQueueDispatch(device->queue, &cmd.base, device->dispatchSemaphore);
    ctsWaitSemaphores(1, &device->dispatchSemaphore);
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
    CtsDevice device,
    CtsDeviceMemory memory,
    CtsDeviceSize offset,
    CtsDeviceSize size,
    CtsMemoryMapFlags flags,
    void** pData
) {
    if (memory == NULL) {
        return CTS_ERROR_MEMORY_MAP_FAILED;
    }

    (void) flags;
    GLuint mapFlags = GL_MAP_READ_BIT  | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
    void* data = glMapBufferRange(GL_COPY_WRITE_BUFFER, offset, size, mapFlags);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    if (data == NULL) {
        return CTS_ERROR_MEMORY_MAP_FAILED;
    }

    *pData = data;
    return CTS_SUCCESS;
}

void ctsUnmapMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory
) {
    if (memory != NULL) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, memory->handle);
        glUnmapBuffer(GL_COPY_WRITE_BUFFER);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }
}

void ctsFreeMemoryImpl(
    CtsDevice device,
    CtsDeviceMemory memory,
    const CtsAllocationCallbacks* pAllocator
) {
    if (memory != NULL) {
        glDeleteBuffers(1, &memory->handle);
        ctsFree(pAllocator, memory);
    }
}

#ifdef __cplusplus
}
#endif