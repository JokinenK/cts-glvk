#pragma once

#include <stdint.h>
#include <cts/typedefs/buffer_view.h>
#include <cts/typedefs/descriptor_pool.h>
#include <cts/typedefs/descriptor_set_layout.h>
#include <cts/typedefs/descriptor.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsDescriptorSet* CtsDescriptorSet;

typedef struct CtsWriteDescriptorSet {
    CtsStructureType sType;
    const void* next;
    CtsDescriptorSet dstSet;
    uint32_t dstBinding;
    uint32_t dstArrayElement;
    uint32_t descriptorCount;
    CtsDescriptorType descriptorType;
    const CtsDescriptorImageInfo* imageInfo;
    const CtsDescriptorBufferInfo* bufferInfo;
    const CtsBufferView* texelBufferView;
} CtsWriteDescriptorSet;

typedef struct CtsCopyDescriptorSet {
    CtsStructureType sType;
    const void* next;
    CtsDescriptorSet srcSet;
    uint32_t srcBinding;
    uint32_t srcArrayElement;
    CtsDescriptorSet dstSet;
    uint32_t dstBinding;
    uint32_t dstArrayElement;
    uint32_t descriptorCount;
} CtsCopyDescriptorSet;

typedef struct CtsDescriptorSetAllocateInfo {
    CtsStructureType sType;
    const void* next;
    CtsDescriptorPool descriptorPool;
    uint32_t descriptorSetCount;
    const CtsDescriptorSetLayout* setLayouts;
} CtsDescriptorSetAllocateInfo;

#ifdef __cplusplus
}
#endif