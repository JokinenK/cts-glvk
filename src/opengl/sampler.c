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
    CtsDevice device,
    const CtsSamplerCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSampler* pSampler
) {
    CtsResult result;
    CtsCreateSampler cmd;
    cmd.base.type = CTS_COMMAND_CREATE_SAMPLER;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pSampler = pSampler;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void ctsDestroySampler(
    CtsDevice device,
    CtsSampler sampler,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroySampler cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_SAMPLER;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.sampler = sampler;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsCreateSamplerImpl(
    CtsDevice device,
    const CtsSamplerCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSampler* pSampler
) {
    (void) device;

    CtsSampler sampler = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSamplerImpl),
        alignof(struct CtsSamplerImpl),
        CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (sampler == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    (void) pCreateInfo->unnormalizedCoordinates;

    glGenSamplers(1, &sampler->handle);
    glSamplerParameteri(sampler->handle, GL_TEXTURE_WRAP_S, parseSamplerAddressMode(pCreateInfo->addressModeU));
    glSamplerParameteri(sampler->handle, GL_TEXTURE_WRAP_T, parseSamplerAddressMode(pCreateInfo->addressModeV));
    glSamplerParameteri(sampler->handle, GL_TEXTURE_WRAP_R, parseSamplerAddressMode(pCreateInfo->addressModeW));
    glSamplerParameteri(sampler->handle, GL_TEXTURE_MIN_FILTER, parseMinFilter(pCreateInfo->minFilter, pCreateInfo->mipmapMode));
    glSamplerParameteri(sampler->handle, GL_TEXTURE_MAG_FILTER, parseMagFilter(pCreateInfo->magFilter, pCreateInfo->mipmapMode));
    glSamplerParameterf(sampler->handle, GL_TEXTURE_MAX_ANISOTROPY, pCreateInfo->anisotropyEnable ? pCreateInfo->maxAnisotropy : 1.0f);
    glSamplerParameterf(sampler->handle, GL_TEXTURE_MIN_LOD, pCreateInfo->minLod);
    glSamplerParameterf(sampler->handle, GL_TEXTURE_MAX_LOD, pCreateInfo->maxLod);
    glSamplerParameterf(sampler->handle, GL_TEXTURE_LOD_BIAS, pCreateInfo->mipLodBias);
    glSamplerParameteri(sampler->handle, GL_TEXTURE_COMPARE_MODE, pCreateInfo->compareEnable ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
    glSamplerParameteri(sampler->handle, GL_TEXTURE_COMPARE_FUNC, parseCompareOp(pCreateInfo->compareOp));

    if (isFloatBorderColor(pCreateInfo->borderColor)) {
        glSamplerParameterfv(sampler->handle, GL_TEXTURE_BORDER_COLOR, parseBorderColorFloat(pCreateInfo->borderColor));
    } else {
        glSamplerParameteriv(sampler->handle, GL_TEXTURE_BORDER_COLOR, parseBorderColorInt(pCreateInfo->borderColor));
    }

    *pSampler = sampler;
    return CTS_SUCCESS;
}

void ctsDestroySamplerImpl(
    CtsDevice device,
    CtsSampler sampler,
    const CtsAllocationCallbacks* pAllocator
) {
    if (sampler != NULL) {
        glDeleteSamplers(1, &sampler->handle);
        ctsFree(pAllocator, sampler);
    }
}

#ifdef __cplusplus
}
#endif