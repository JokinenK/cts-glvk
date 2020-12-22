#pragma once

#include <cts/typedefs/buffer_view.h>
#include <cts/typedefs/descriptor_set.h>
#include <cts/typedefs/descriptor.h>
#include <cts/typedefs/gl_enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlDescriptorSetLayoutBinding {
    uint32_t binding;
    CtsGlDescriptorType type;
    CtsDescriptorType descriptorType;
    uint32_t descriptorCount;
    CtsShaderStageFlags stageFlags;
    //const CtsSampler* immutableSamplers;
    CtsDescriptorImageInfo* imageInfo;
    CtsDescriptorBufferInfo* bufferInfo;
    CtsBufferView* texelBufferView;
} CtsGlDescriptorSetLayoutBinding;

struct CtsDescriptorSet {
    uint32_t bindingCount;
    CtsGlDescriptorSetLayoutBinding* bindings;
};

#ifdef __cplusplus
}
#endif