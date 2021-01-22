#pragma once

#include <cts/typedefs/descriptor.h>
#include <cts/typedefs/buffer.h>
#include <cts/typedefs/buffer_view.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/image_view.h>
#include <cts/typedefs/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlDescriptorImageView {
    CtsSampler sampler;
    CtsImageView imageView;
} CtsGlDescriptorImageView;

typedef struct CtsGlDescriptorBufferView {
    CtsBufferView bufferView;
} CtsGlDescriptorBufferView;

typedef struct CtsGlDescriptorBuffer {
    CtsBuffer buffer;
    size_t offset;
    size_t range; 
} CtsGlDescriptorBuffer;

typedef struct CtsGlDescriptor {
    CtsDescriptorType type;

    union {
        CtsGlDescriptorImageView imageViewContainer;
        CtsGlDescriptorBufferView bufferViewContainer;
        CtsGlDescriptorBuffer bufferContainer;
    };
} CtsGlDescriptor;

#ifdef __cplusplus
}
#endif