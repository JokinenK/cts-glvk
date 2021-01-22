#include <assert.h>
#include <stddef.h>
#include <cts/descriptor_set.h>
#include <cts/commands.h>
#include <cts/type_mapper.h>
#include <private/device_private.h>
#include <private/descriptor_pool_private.h>
#include <private/descriptor_set_layout_private.h>
#include <private/descriptor_set_private.h>
#include <private/queue_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static void writeImageView(
    CtsDevice device,
    CtsDescriptorSet descriptorSet, 
    CtsDescriptorType descriptorType, 
    const CtsDescriptorImageInfo* pDescriptorImageInfo, 
    uint32_t binding, 
    uint32_t element
);

static void writeBufferView(
    CtsDevice device,
    CtsDescriptorSet descriptorSet, 
    CtsDescriptorType descriptorType, 
    CtsBufferView pBufferView, 
    uint32_t binding, 
    uint32_t element
);

static void writeBuffer(
    CtsDevice device,
    CtsDescriptorSet descriptorSet, 
    CtsDescriptorType descriptorType, 
    const CtsDescriptorBufferInfo* pDescriptorBufferInfo, 
    uint32_t binding, 
    uint32_t element
);

CtsResult ctsAllocateDescriptorSets(
    CtsDevice device,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
) {
    CtsResult result;
    CtsAllocateDescriptorSets cmd;
    cmd.base.type = CTS_COMMAND_ALLOCATE_DESCRIPTOR_SETS;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.pAllocateInfo = pAllocateInfo;
    cmd.pDescriptorSets = pDescriptorSets;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}
 
void ctsUpdateDescriptorSets(
    CtsDevice device,
    uint32_t descriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
) {
    CtsUpdateDescriptorSets cmd;
    cmd.base.type = CTS_COMMAND_UPDATE_DESCRIPTOR_SETS;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.descriptorWriteCount = descriptorWriteCount;
    cmd.pDescriptorWrites = pDescriptorWrites;
    cmd.descriptorCopyCount = descriptorCopyCount;
    cmd.pDescriptorCopies = pDescriptorCopies;

    ctsQueueDispatch(device->queue, &cmd.base);
}

CtsResult ctsFreeDescriptorSets(
    CtsDevice device,
    CtsDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
) {
    CtsResult result;
    CtsFreeDescriptorSets cmd;
    cmd.base.type = CTS_COMMAND_FREE_DESCRIPTOR_SETS;
    cmd.base.pNext = NULL;

    cmd.device = device;
    cmd.descriptorPool = descriptorPool;
    cmd.descriptorSetCount = descriptorSetCount;
    cmd.pDescriptorSets = pDescriptorSets;
    cmd.pResult = &result;

    ctsQueueDispatch(device->queue, &cmd.base);

    return result;
}

CtsResult ctsAllocateDescriptorSetsImpl(
    CtsDevice device,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
) {
    (void) device;

    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    for (; i < pAllocateInfo->descriptorSetCount; ++i) {
        CtsDescriptorSet descriptorSet = ctsAllocation(
            &pAllocateInfo->descriptorPool->allocator,
            sizeof(struct CtsDescriptorSetImpl),
            alignof(struct CtsDescriptorSetImpl),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );
        
        if (descriptorSet == NULL) {
            result = CTS_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        CtsDescriptorSetLayout layout = pAllocateInfo->pSetLayouts[i];
        
        uint32_t descriptorCount = 0;
        for (uint32_t j = 0; j < layout->bindingCount; ++j) {
            const CtsGlDescriptorSetLayoutBinding* binding = &layout->bindings[j];
            descriptorCount += binding->descriptorCount;
        }

        descriptorSet->layout = layout;
        descriptorSet->descriptorCount = descriptorCount;
        descriptorSet->descriptors = ctsAllocation(
            &pAllocateInfo->descriptorPool->allocator,
            sizeof(CtsGlDescriptor) * descriptorCount,
            alignof(CtsGlDescriptor),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        if (descriptorSet->descriptors == NULL) {
            result = CTS_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        pDescriptorSets[i] = descriptorSet;
    }

    for (; i < pAllocateInfo->descriptorSetCount; ++i) {
        pDescriptorSets[i] = NULL;
    }

    return result;
}

void ctsUpdateDescriptorSetsImpl(
    CtsDevice device,
    uint32_t descriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
) {
    for (uint32_t i = 0; i < descriptorWriteCount; ++i) {
        const CtsWriteDescriptorSet* write = &pDescriptorWrites[i];
        CtsDescriptorSet target = write->dstSet;
        CtsGlDescriptorSetLayoutBinding* binding = &target->layout->bindings[write->dstBinding];
        uint32_t offset = 0;

        switch (write->descriptorType) {
            case CTS_DESCRIPTOR_TYPE_SAMPLER:
            case CTS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case CTS_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case CTS_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case CTS_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
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

            case CTS_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case CTS_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
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

            case CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case CTS_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            case CTS_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
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
        const CtsCopyDescriptorSet* descriptorSet = &pDescriptorCopies[i];

        CtsDescriptorSet src = descriptorSet->srcSet;
        CtsDescriptorSet dst = descriptorSet->dstSet;

        CtsGlDescriptorSetLayoutBinding* srcBinding = &src->layout->bindings[descriptorSet->srcBinding];
        CtsGlDescriptorSetLayoutBinding* dstBinding = &dst->layout->bindings[descriptorSet->dstBinding];

        uint32_t srcOffset = 0;
        uint32_t dstOffset = 0;

        for (uint32_t j = 0; j < descriptorSet->descriptorCount; ++j) {
            srcOffset = srcBinding->descriptorOffset + descriptorSet->srcArrayElement + j;
            dstOffset = dstBinding->descriptorOffset + descriptorSet->dstArrayElement + j;
            dst->descriptors[dstOffset] = src->descriptors[srcOffset];
        }
    }
}

CtsResult ctsFreeDescriptorSetsImpl(
    CtsDevice device,
    CtsDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
) {
    for (uint32_t i = 0; descriptorSetCount; ++i) {
        ctsFree(&descriptorPool->allocator, pDescriptorSets[i]);
    }

    return CTS_SUCCESS;
}

static void writeImageView(
    CtsDevice device,
    CtsDescriptorSet descriptorSet, 
    CtsDescriptorType descriptorType, 
    const CtsDescriptorImageInfo* pDescriptorImageInfo, 
    uint32_t binding, 
    uint32_t element
) {
    const CtsGlDescriptorSetLayoutBinding* bindLayout = &descriptorSet->layout->bindings[binding];
    CtsGlDescriptor* descriptor = &descriptorSet->descriptors[element];

    assert(descriptorType == bindLayout->descriptorType || descriptorType == CTS_DESCRIPTOR_TYPE_SAMPLER);

    CtsImageView imageView = NULL;
    CtsSampler sampler = NULL;

    switch (descriptorType) {
        case CTS_DESCRIPTOR_TYPE_SAMPLER: {
            sampler = bindLayout->immutableSamplers
                ? bindLayout->immutableSamplers[element]
                : pDescriptorImageInfo->sampler;
        } break;

        case CTS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
            imageView = pDescriptorImageInfo->imageView;
            sampler = bindLayout->immutableSamplers
                ? bindLayout->immutableSamplers[element]
                : pDescriptorImageInfo->sampler;
        } break;

        case CTS_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case CTS_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        case CTS_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
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
    CtsDevice device,
    CtsDescriptorSet descriptorSet, 
    CtsDescriptorType descriptorType, 
    CtsBufferView bufferView, 
    uint32_t binding, 
    uint32_t element
) {
    const CtsGlDescriptorSetLayoutBinding* bindLayout = &descriptorSet->layout->bindings[binding];
    CtsGlDescriptor* descriptor = &descriptorSet->descriptors[element];

    assert(descriptorType == bindLayout->descriptorType);

    descriptor->type = descriptorType;
    descriptor->bufferViewContainer.bufferView = bufferView;
}

static void writeBuffer(
    CtsDevice device,
    CtsDescriptorSet descriptorSet, 
    CtsDescriptorType descriptorType, 
    const CtsDescriptorBufferInfo* pDescriptorBufferInfo, 
    uint32_t binding, 
    uint32_t element
) {
    const CtsGlDescriptorSetLayoutBinding* bindLayout = &descriptorSet->layout->bindings[binding];
    CtsGlDescriptor* descriptor = &descriptorSet->descriptors[element];

    assert(descriptorType == bindLayout->descriptorType);

    CtsBuffer buffer = NULL;
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