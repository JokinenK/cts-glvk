#pragma once

#include <cts/typedefs/buffer.h>
#include <cts/typedefs/buffer_view.h>
#include <cts/typedefs/enums.h>
#include <cts/typedefs/image_view.h>
#include <cts/typedefs/sampler.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDescriptorImageView {
    CtsSampler sampler;
    CtsImageView imageView;
} CtsDescriptorImageView;

typedef struct CtsDescriptorBufferView {
    CtsBufferView bufferView;
} CtsDescriptorBufferView;

typedef struct CtsDescriptorBuffer {
    CtsBuffer buffer;
    size_t offset;
    size_t range; 
} CtsDescriptorBuffer;

struct CtsDescriptor {
    CtsDescriptorType type;

    union {
        CtsDescriptorImageView imageViewContainer;
        CtsDescriptorBufferView bufferViewContainer;
        CtsDescriptorBuffer bufferContainer;
    };
};

#ifdef __cplusplus
}
#endif