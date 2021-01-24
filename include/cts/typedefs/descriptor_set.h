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

typedef struct CtsDescriptorSetImpl* CtsDescriptorSet;

typedef struct CtsWriteDescriptorSet {
    CtsStructureType                sType;
    const void*                     pNext;
    CtsDescriptorSet                dstSet;
    uint32_t                        dstBinding;
    uint32_t                        dstArrayElement;
    uint32_t                        descriptorCount;
    CtsDescriptorType               descriptorType;
    const CtsDescriptorImageInfo*   pImageInfo;
    const CtsDescriptorBufferInfo*  pBufferInfo;
    const CtsBufferView*            pTexelBufferView;
} CtsWriteDescriptorSet;

typedef struct CtsCopyDescriptorSet {
    CtsStructureType    sType;
    const void*         pNext;
    CtsDescriptorSet    srcSet;
    uint32_t            srcBinding;
    uint32_t            srcArrayElement;
    CtsDescriptorSet    dstSet;
    uint32_t            dstBinding;
    uint32_t            dstArrayElement;
    uint32_t            descriptorCount;
} CtsCopyDescriptorSet;

typedef struct CtsDescriptorSetAllocateInfo {
    CtsStructureType                sType;
    const void*                     pNext;
    CtsDescriptorPool               descriptorPool;
    uint32_t                        descriptorSetCount;
    const CtsDescriptorSetLayout*   pSetLayouts;
} CtsDescriptorSetAllocateInfo;

#ifdef __cplusplus
}
#endif