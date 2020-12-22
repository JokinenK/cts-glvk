#pragma once

#include <cts/typedefs/image.h>
#include <cts/typedefs/sampler.h>
    
#ifdef __cplusplus
extern "C" {
#endif

struct CtsImageView {
    GLenum target;
    GLenum handle;

    CtsImageViewType viewType;
    CtsSampler activeSampler;
};

#ifdef __cplusplus
}
#endif
