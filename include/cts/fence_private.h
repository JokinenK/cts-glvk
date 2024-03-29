#pragma once

#include <stdbool.h>
#include "glad/glad.h"
#include "cts/fence.h"
#include "cts/object_base.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CtsFence {
    struct CtsObjectBase base;
    GLsync sync;
    GLint status;
};

void ctsSignalFence(
    VkDevice device,
    VkFence fence
);

VkResult ctsCreateFenceImpl(
    VkDevice device,
    const VkFenceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence
);

VkResult ctsResetFencesImpl(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences
);

VkResult ctsGetFenceStatusImpl(
    VkDevice device,
    VkFence fence
);

void ctsSignalFenceImpl(
    VkDevice device,
    VkFence fence
);

VkResult ctsWaitForFencesImpl(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout
);

void ctsDestroyFenceImpl(
    VkDevice device,
    VkFence fence,
    const VkAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif