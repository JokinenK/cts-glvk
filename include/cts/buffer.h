#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateBuffer(
    CtsDevice device,
    const CtsBufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsBuffer* pBuffer
);

CtsResult ctsBindBufferMemory(
    CtsDevice device,
    CtsBuffer buffer, 
    CtsDeviceMemory memory,
    CtsDeviceSize offset
);

void ctsDestroyBuffer(
    CtsDevice device,
    CtsBuffer buffer,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif