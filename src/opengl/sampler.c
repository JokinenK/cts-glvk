#include <stddef.h>
#include <cts/sampler.h>
#include <cts/type_mapper.h>
#include <cts/commands.h>
#include <private/device_private.h>
#include <private/sampler_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

CtsResult ctsCreateSampler(
    CtsDevice pDevice,
    const CtsSamplerCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSampler* pSampler
) {
    CtsResult result;
    CtsCreateSampler cmd;
    cmd.base.type = CTS_COMMAND_CREATE_SAMPLER;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.sampler = pSampler;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

void ctsDestroySampler(
    CtsDevice pDevice,
    CtsSampler pSampler,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroySampler cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_SAMPLER;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.sampler = pSampler;
    cmd.allocator = pAllocator;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);
}

CtsResult ctsCreateSamplerImpl(
    CtsDevice pDevice,
    const CtsSamplerCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSampler* pSampler
) {
    (void) pDevice;

    CtsSampler sampler = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSampler),
        alignof(struct CtsSampler),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (sampler == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    (void) pCreateInfo->compareEnable;
    (void) pCreateInfo->compareOp;
    (void) pCreateInfo->maxAnisotropy;
    (void) pCreateInfo->maxLod;
    (void) pCreateInfo->minLod;
    (void) pCreateInfo->mipLodBias;
    (void) pCreateInfo->mipmapMode;
    (void) pCreateInfo->unnormalizedCoordinates;

    glGenSamplers(1, &sampler->handle);
    glSamplerParameteri(sampler->handle, GL_TEXTURE_WRAP_S, parseSamplerAddressMode(pCreateInfo->addressModeU));
    glSamplerParameteri(sampler->handle, GL_TEXTURE_WRAP_T, parseSamplerAddressMode(pCreateInfo->addressModeV));
    glSamplerParameteri(sampler->handle, GL_TEXTURE_WRAP_R, parseSamplerAddressMode(pCreateInfo->addressModeW));
    glSamplerParameteri(sampler->handle, GL_TEXTURE_MIN_FILTER, parseFilter(pCreateInfo->minFilter));
    glSamplerParameteri(sampler->handle, GL_TEXTURE_MAG_FILTER, parseFilter(pCreateInfo->magFilter));

    *pSampler = sampler;
    return CTS_SUCCESS;
}

void ctsDestroySamplerImpl(
    CtsDevice pDevice,
    CtsSampler pSampler,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pSampler != NULL) {
        glDeleteSamplers(1, &pSampler->handle);
        ctsFree(pAllocator, pSampler);
    }
}

#ifdef __cplusplus
}
#endif