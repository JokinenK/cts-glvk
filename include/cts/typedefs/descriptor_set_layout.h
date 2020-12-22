#pragma once

#include <cts/typedefs/enums.h>
#include <cts/typedefs/sampler.h>
#include <cts/typedefs/shader_stage_flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDescriptorSetLayoutBinding {
    uint32_t binding;
    CtsDescriptorType descriptorType;
    uint32_t descriptorCount;
    CtsShaderStageFlags stageFlags;
    const CtsSampler* immutableSamplers;
} CtsDescriptorSetLayoutBinding;

typedef struct CtsDescriptorSetLayout* CtsDescriptorSetLayout;
typedef struct CtsDescriptorSetLayoutCreateInfo {
    CtsStructureType sType;
    const void* next;
    uint32_t bindingCount;
    const CtsDescriptorSetLayoutBinding* bindings;
} CtsDescriptorSetLayoutCreateInfo;

#ifdef __cplusplus
}
#endif