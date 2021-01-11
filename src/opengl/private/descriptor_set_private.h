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

CtsResult ctsAllocateDescriptorSetsImpl(
    CtsDevice pDevice,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
);

void ctsUpdateDescriptorSetsImpl(
    CtsDevice pDevice,
    uint32_t pDescriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t pDescriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
);

CtsResult ctsFreeDescriptorSetsImpl(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
);

#ifdef __cplusplus
}
#endif