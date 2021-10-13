#include "cts/util/align.h"
#include "cts/allocator.h"
#include <cts/semaphore.h>
#include <private/private.h>
#include <private/semaphore_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateSemaphore(
    VkDevice deviceHandle,
    const VkSemaphoreCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSemaphore* pSemaphore
) {
    (void) deviceHandle;

    struct CtsSemaphore* semaphore = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSemaphore),
        alignof(struct CtsSemaphore),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (semaphore == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(semaphore);
    ctsInitPlatformSemaphore(&semaphore->semaphore, 0, 1);

    *pSemaphore = CtsSemaphoreToHandle(semaphore);
    return VK_SUCCESS;
}

void VKAPI_CALL ctsDestroySemaphore(
    VkDevice deviceHandle,
    VkSemaphore semaphoreHandle,
    const VkAllocationCallbacks* pAllocator
) {
    (void) deviceHandle;

    struct CtsSemaphore* semaphore = CtsSemaphoreFromHandle(semaphoreHandle);

    if (semaphore != NULL) {
        ctsDestroyPlatformSemaphore(&semaphore->semaphore);
        ctsFree(pAllocator, semaphore);
    }
}

VkResult ctsWaitSemaphore(
    VkSemaphore semaphoreHandle,
    uint64_t timeout
) {
    struct CtsSemaphore* semaphore = CtsSemaphoreFromHandle(semaphoreHandle);

    if (ctsWaitPlatformSemaphore(&semaphore->semaphore, timeout)) {
        return VK_SUCCESS;
    }

    return VK_TIMEOUT;
}

void ctsWaitSemaphores(
    uint32_t semaphoreCount,
    const VkSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        struct CtsSemaphore* semaphore = CtsSemaphoreFromHandle(pSemaphores[i]);
        ctsWaitPlatformSemaphore(&semaphore->semaphore, UINT64_MAX);
    }
}

void ctsSignalSemaphores(
    uint32_t semaphoreCount,
    const VkSemaphore* pSemaphores
) {
    for (uint32_t i = 0; i < semaphoreCount; ++i) {
        struct CtsSemaphore* semaphore = CtsSemaphoreFromHandle(pSemaphores[i]);
        ReleaseSemaphore(semaphore->semaphore, 1, NULL);
    }
}

#ifdef __cplusplus
}
#endif