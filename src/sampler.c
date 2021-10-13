#include <stddef.h>
#include <cts/sampler.h>
#include <cts/type_mapper.h>
#include <cts/commands.h>
#include <private/private.h>
#include <private/device_private.h>
#include <private/sampler_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

VkResult VKAPI_CALL ctsCreateSampler(
    VkDevice deviceHandle,
    const VkSamplerCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSampler* pSampler
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsCreateSampler cmd;
    cmd.base.type = CTS_COMMAND_CREATE_SAMPLER;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.pCreateInfo = pCreateInfo;
    cmd.pAllocator = pAllocator;
    cmd.pSampler = pSampler;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

void VKAPI_CALL ctsDestroySampler(
    VkDevice deviceHandle,
    VkSampler sampler,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsDestroySampler cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_SAMPLER;
    cmd.base.pNext = NULL;

    cmd.device = deviceHandle;
    cmd.sampler = sampler;
    cmd.pAllocator = pAllocator;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult ctsCreateSamplerImpl(
    VkDevice deviceHandle,
    const VkSamplerCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSampler* pSampler
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsSampler* sampler = ctsAllocation(
        pAllocator,
        sizeof(struct CtsSampler),
        alignof(struct CtsSampler),
        VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
    );

    if (sampler == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    set_loader_magic_value(sampler);

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

    *pSampler = CtsSamplerToHandle(sampler);
    return VK_SUCCESS;
}

void ctsDestroySamplerImpl(
    VkDevice deviceHandle,
    VkSampler samplerHandle,
    const VkAllocationCallbacks* pAllocator
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);
    struct CtsSampler* sampler = CtsSamplerFromHandle(samplerHandle);

    if (sampler != NULL) {
        glDeleteSamplers(1, &sampler->handle);
        ctsFree(pAllocator, sampler);
    }
}

#ifdef __cplusplus
}
#endif