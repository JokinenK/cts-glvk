#include <cts/types.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateBuffer(
    CtsDevice pDevice,
    const CtsBufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsBuffer* pBuffer
);

CtsResult ctsBindBufferMemory(
    CtsDevice pDevice,
    CtsBuffer pBuffer, 
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset
);

void ctsDestroyBuffer(
    CtsDevice pDevice,
    CtsBuffer pBuffer,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif