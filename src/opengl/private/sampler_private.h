#pragma once

#include <glad/glad.h>
#include <cts/typedefs/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSamplerImpl {
    GLuint handle;
};

CtsResult ctsCreateSamplerImpl(
    CtsDevice device,
    const CtsSamplerCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSampler* pSampler
);

void ctsDestroySamplerImpl(
    CtsDevice device,
    CtsSampler sampler,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif