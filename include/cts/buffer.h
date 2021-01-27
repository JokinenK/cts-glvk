#include <cts/allocator.h>
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

void ctsDestroyBuffer(
    CtsDevice device,
    CtsBuffer buffer,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif