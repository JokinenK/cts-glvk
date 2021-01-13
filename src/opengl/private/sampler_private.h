#pragma once

#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSampler {
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