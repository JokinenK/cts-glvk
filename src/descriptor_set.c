#include <assert.h>
#include <stddef.h>
#include "cts/descriptor_set.h"
#include "cts/commands.h"
#include "cts/type_mapper.h"
#include "cts/private.h"
#include "cts/device_private.h"
#include "cts/descriptor_pool_private.h"
#include "cts/descriptor_set_layout_private.h"
#include "cts/descriptor_set_private.h"
#include "cts/queue_private.h"

#ifdef __cplusplus
extern "C" {
#endif

static void writeImageView(
    struct CtsDevice* device,
    struct CtsDescriptorSet* descriptorSet, 
    VkDescriptorType descriptorType, 
    const VkDescriptorImageInfo* pDescriptorImageInfo, 
    uint32_t binding, 
    uint32_t element
);

static void writeBufferView(
    struct CtsDevice* device,
    struct CtsDescriptorSet* descriptorSet, 
    VkDescriptorType descriptorType, 
    VkBufferView pBufferView, 
    uint32_t binding, 
    uint32_t element
);

static void writeBuffer(
    struct CtsDevice* device,
    struct CtsDescriptorSet* descriptorSet, 
    VkDescriptorType descriptorType, 
    const VkDescriptorBufferInfo* pDescriptorBufferInfo, 
    uint32_t binding, 
    uint32_t element
);

VkResult VKAPI_CALL ctsAllocateDescriptorSets(
    VkDevice deviceHandle,
    const VkDescriptorSetAllocateInfo* pAllocateInfo,
    VkDescriptorSet* pDescriptorSets
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsAllocateDescriptorSets cmd;
    cmd.base.pNext = NULL;
    cmd.base.pMetadata = CTS_COMMAND_METADATA(CtsAllocateDescriptorSets);

    cmd.device = deviceHandle;
    cmd.pAllocateInfo = pAllocateInfo;
    cmd.pDescriptorSets = pDescriptorSets;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}
 
void VKAPI_CALL ctsUpdateDescriptorSets(
    VkDevice deviceHandle,
    uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const VkCopyDescriptorSet* pDescriptorCopies
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    CtsUpdateDescriptorSets cmd;
    cmd.base.pNext = NULL;
    cmd.base.pMetadata = CTS_COMMAND_METADATA(CtsUpdateDescriptorSets);

    cmd.device = deviceHandle;
    cmd.descriptorWriteCount = descriptorWriteCount;
    cmd.pDescriptorWrites = pDescriptorWrites;
    cmd.descriptorCopyCount = descriptorCopyCount;
    cmd.pDescriptorCopies = pDescriptorCopies;

    ctsQueueDispatch(device->queue, &cmd.base);
}

VkResult VKAPI_CALL ctsFreeDescriptorSets(
    VkDevice deviceHandle,
    VkDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    VkResult result;
    CtsFreeDescriptorSets cmd;
    cmd.base.pNext = NULL;
    cmd.base.pMetadata = CTS_COMMAND_METADATA(CtsFreeDescriptorSets);

    cmd.device = deviceHandle;
    cmd.descriptorPool = descriptorPool;
    cmd.descriptorSetCount = descriptorSetCount;
    cmd.pDescriptorSets = pDescriptorSets;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);
    return result;
}

VkResult ctsAllocateDescriptorSetsImpl(
    VkDevice device,
    const VkDescriptorSetAllocateInfo* pAllocateInfo,
    VkDescriptorSet* pDescriptorSets
) {
    (void) device;

    VkResult result = VK_SUCCESS;
    uint32_t i = 0;

    struct CtsDescriptorPool* descriptorPool = CtsDescriptorPoolFromHandle(pAllocateInfo->descriptorPool);

    for (; i < pAllocateInfo->descriptorSetCount; ++i) {
        struct CtsDescriptorSet* descriptorSet = ctsAllocation(
            &descriptorPool->allocator,
            sizeof(struct CtsDescriptorSet),
            alignof(struct CtsDescriptorSet),
            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );
        
        if (descriptorSet == NULL) {
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        struct CtsDescriptorSetLayout* layout = CtsDescriptorSetLayoutFromHandle(pAllocateInfo->pSetLayouts[i]);
        
        uint32_t descriptorCount = 0;
        for (uint32_t j = 0; j < layout->bindingCount; ++j) {
            const struct CtsGlDescriptorSetLayoutBinding* binding = &layout->bindings[j];
            descriptorCount += binding->descriptorCount;
        }

        descriptorSet->layout = layout;
        descriptorSet->descriptorCount = descriptorCount;
        descriptorSet->descriptors = ctsAllocation(
            &descriptorPool->allocator,
            sizeof(CtsGlDescriptor) * descriptorCount,
            alignof(CtsGlDescriptor),
            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (descriptorSet->descriptors == NULL) {
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        pDescriptorSets[i] = CtsDescriptorSetToHandle(descriptorSet);
    }

    for (; i < pAllocateInfo->descriptorSetCount; ++i) {
        pDescriptorSets[i] = NULL;
    }

    return result;
}

void ctsUpdateDescriptorSetsImpl(
    VkDevice deviceHandle,
    uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const VkCopyDescriptorSet* pDescriptorCopies
) {
    struct CtsDevice* device = CtsDeviceFromHandle(deviceHandle);

    for (uint32_t i = 0; i < descriptorWriteCount; ++i) {
        const VkWriteDescriptorSet* write = &pDescriptorWrites[i];
        struct CtsDescriptorSet* target = CtsDescriptorSetFromHandle(write->dstSet);
        struct CtsGlDescriptorSetLayoutBinding* binding = &target->layout->bindings[write->dstBinding];
        uint32_t offset = 0;

        switch (write->descriptorType) {
            case VK_DESCRIPTOR_TYPE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                for (uint32_t j = 0; j < write->descriptorCount; j++) {
                    offset = binding->descriptorOffset + write->dstArrayElement + j;
                    writeImageView(
                        device,
                        target,
                        write->descriptorType,
                        &write->pImageInfo[j],
                        write->dstBinding,
                        offset
                    );
                }
                break;

            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                for (uint32_t j = 0; j < write->descriptorCount; j++) {
                    offset = binding->descriptorOffset + write->dstArrayElement + j;
                    writeBufferView(
                        device,
                        target,
                        write->descriptorType,
                        write->pTexelBufferView[j],
                        write->dstBinding,
                        offset
                    );
                }
                break;

            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                for (uint32_t j = 0; j < write->descriptorCount; j++) {
                    offset = binding->descriptorOffset + write->dstArrayElement + j;
                    writeBuffer(
                        device,
                        target,
                        write->descriptorType,
                        &write->pBufferInfo[j],
                        write->dstBinding,
                        offset
                    );
                }
                break;

            default:
                break;
        }
    }

    for (uint32_t i = 0; i < descriptorCopyCount; ++i) {
        const VkCopyDescriptorSet* descriptorSet = &pDescriptorCopies[i];

        struct CtsDescriptorSet* src = CtsDescriptorSetFromHandle(descriptorSet->srcSet);
        struct CtsDescriptorSet* dst = CtsDescriptorSetFromHandle(descriptorSet->dstSet);

        struct CtsGlDescriptorSetLayoutBinding* srcBinding = &src->layout->bindings[descriptorSet->srcBinding];
        struct CtsGlDescriptorSetLayoutBinding* dstBinding = &dst->layout->bindings[descriptorSet->dstBinding];

        uint32_t srcOffset = 0;
        uint32_t dstOffset = 0;

        for (uint32_t j = 0; j < descriptorSet->descriptorCount; ++j) {
            srcOffset = srcBinding->descriptorOffset + descriptorSet->srcArrayElement + j;
            dstOffset = dstBinding->descriptorOffset + descriptorSet->dstArrayElement + j;
            dst->descriptors[dstOffset] = src->descriptors[srcOffset];
        }
    }
}

VkResult ctsFreeDescriptorSetsImpl(
    VkDevice device,
    VkDescriptorPool descriptorPoolHandle,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets
) {
    struct CtsDescriptorPool* descriptorPool = CtsDescriptorPoolFromHandle(descriptorPoolHandle);

    for (uint32_t i = 0; descriptorSetCount; ++i) {
        ctsFree(&descriptorPool->allocator, pDescriptorSets[i]);
    }

    return VK_SUCCESS;
}

static void writeImageView(
    struct CtsDevice* device,
    struct CtsDescriptorSet* descriptorSet, 
    VkDescriptorType descriptorType, 
    const VkDescriptorImageInfo* pDescriptorImageInfo, 
    uint32_t binding, 
    uint32_t element
) {
    (void) device;
    
    const struct CtsGlDescriptorSetLayoutBinding* bindLayout = &descriptorSet->layout->bindings[binding];
    CtsGlDescriptor* descriptor = &descriptorSet->descriptors[element];

    assert(descriptorType == bindLayout->descriptorType || descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER);

    VkImageView imageView = NULL;
    VkSampler sampler = NULL;

    switch (descriptorType) {
        case VK_DESCRIPTOR_TYPE_SAMPLER: {
            sampler = bindLayout->immutableSamplers
                ? bindLayout->immutableSamplers[element]
                : pDescriptorImageInfo->sampler;
        } break;

        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
            imageView = pDescriptorImageInfo->imageView;
            sampler = bindLayout->immutableSamplers
                ? bindLayout->immutableSamplers[element]
                : pDescriptorImageInfo->sampler;
        } break;

        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
            imageView = pDescriptorImageInfo->imageView;
        } break;

        default: {
            /* Unreachable */
        } break;
    }
    
    descriptor->type = descriptorType;
    descriptor->imageViewContainer.imageView = imageView;
    descriptor->imageViewContainer.sampler = sampler;
}


static void writeBufferView(
    struct CtsDevice* device,
    struct CtsDescriptorSet* descriptorSet, 
    VkDescriptorType descriptorType, 
    VkBufferView bufferView, 
    uint32_t binding, 
    uint32_t element
) {
    (void) device;

    const struct CtsGlDescriptorSetLayoutBinding* bindLayout = &descriptorSet->layout->bindings[binding];
    CtsGlDescriptor* descriptor = &descriptorSet->descriptors[element];

    assert(descriptorType == bindLayout->descriptorType);

    descriptor->type = descriptorType;
    descriptor->bufferViewContainer.bufferView = bufferView;
}

static void writeBuffer(
    struct CtsDevice* device,
    struct CtsDescriptorSet* descriptorSet, 
    VkDescriptorType descriptorType, 
    const VkDescriptorBufferInfo* pDescriptorBufferInfo, 
    uint32_t binding, 
    uint32_t element
) {
    (void) device;

    const struct CtsGlDescriptorSetLayoutBinding* bindLayout = &descriptorSet->layout->bindings[binding];
    CtsGlDescriptor* descriptor = &descriptorSet->descriptors[element];

    assert(descriptorType == bindLayout->descriptorType);

    VkBuffer buffer = NULL;
    size_t offset = 0;
    size_t range = 0;

    if (pDescriptorBufferInfo != NULL) {
        buffer = pDescriptorBufferInfo->buffer;
        offset = pDescriptorBufferInfo->offset;
        range = pDescriptorBufferInfo->range;
    }

    descriptor->type = descriptorType;
    descriptor->bufferContainer.buffer = buffer;
    descriptor->bufferContainer.offset = offset;
    descriptor->bufferContainer.range = range;
}

#ifdef __cplusplus
}
#endif