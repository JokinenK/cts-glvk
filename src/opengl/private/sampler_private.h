#pragma once

#include <glad/glad.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsSampler {
    GLuint handle;
};

CtsResult ctsCreateSamplerImpl(
    CtsDevice pDevice,
    const CtsSamplerCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsSampler* pSampler
);

void ctsDestroySamplerImpl(
    CtsDevice pDevice,
    CtsSampler pSampler,
    const CtsAllocationCallbacks* pAllocator
);

#ifdef __cplusplus
}
#endif