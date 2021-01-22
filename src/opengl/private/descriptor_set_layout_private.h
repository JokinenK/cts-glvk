#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/descriptor_set_layout.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlDescriptorSetLayoutBinding {
    uint32_t binding;
    CtsDescriptorType descriptorType;
    uint32_t descriptorCount;
    uint32_t descriptorOffset;
    CtsShaderStageFlags stageFlags;
    const CtsSampler* immutableSamplers;
} CtsGlDescriptorSetLayoutBinding;

struct CtsDescriptorSetLayoutImpl {
    uint32_t bindingCount;
    CtsGlDescriptorSetLayoutBinding* bindings;
};

#ifdef __cplusplus
}
#endif