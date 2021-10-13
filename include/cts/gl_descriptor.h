#pragma once

#include "vulkan/vulkan_core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlDescriptorImageView {
    VkSampler sampler;
    VkImageView imageView;
} CtsGlDescriptorImageView;

typedef struct CtsGlDescriptorBufferView {
    VkBufferView bufferView;
} CtsGlDescriptorBufferView;

typedef struct CtsGlDescriptorBuffer {
    VkBuffer buffer;
    size_t offset;
    size_t range; 
} CtsGlDescriptorBuffer;

typedef struct CtsGlDescriptor {
    VkDescriptorType type;

    union {
        CtsGlDescriptorImageView imageViewContainer;
        CtsGlDescriptorBufferView bufferViewContainer;
        CtsGlDescriptorBuffer bufferContainer;
    };
} CtsGlDescriptor;

#ifdef __cplusplus
}
#endif