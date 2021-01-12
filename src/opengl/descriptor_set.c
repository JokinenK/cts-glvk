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

CtsResult ctsAllocateDescriptorSets(
    CtsDevice pDevice,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
) {
    CtsResult result;
    CtsAllocateDescriptorSets cmd;
    cmd.base.type = CTS_COMMAND_ALLOCATE_DESCRIPTOR_SETS;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.allocateInfo = pAllocateInfo;
    cmd.descriptorSets = pDescriptorSets;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

void ctsUpdateDescriptorSets(
    CtsDevice pDevice,
    uint32_t pDescriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t pDescriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
) {
    CtsUpdateDescriptorSets cmd;
    cmd.base.type = CTS_COMMAND_UPDATE_DESCRIPTOR_SETS;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.descriptorWriteCount = pDescriptorWriteCount;
    cmd.descriptorWrites = pDescriptorWrites;
    cmd.descriptorCopyCount = pDescriptorCopyCount;
    cmd.descriptorCopies = pDescriptorCopies;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);
}

CtsResult ctsFreeDescriptorSets(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
) {
    CtsResult result;
    CtsFreeDescriptorSets cmd;
    cmd.base.type = CTS_COMMAND_FREE_DESCRIPTOR_SETS;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.descriptorPool = pDescriptorPool;
    cmd.descriptorSetCount = pDescriptorSetCount;
    cmd.descriptorSets = pDescriptorSets;
    cmd.result = &result;

    ctsQueueDispatch(pDevice->queue, &cmd.base, pDevice->dispatchSemaphore);
    ctsWaitSemaphores(1, &pDevice->dispatchSemaphore);

    return result;
}

CtsResult ctsAllocateDescriptorSetsImpl(
    CtsDevice pDevice,
    const CtsDescriptorSetAllocateInfo* pAllocateInfo,
    CtsDescriptorSet* pDescriptorSets
) {
    (void) pDevice;

    CtsResult result = CTS_SUCCESS;
    uint32_t i = 0;

    for (; i < pAllocateInfo->descriptorSetCount; ++i) {
        CtsDescriptorSet descriptorSet = ctsDescriptorPoolAllocateSet(pAllocateInfo->descriptorPool);
        if (descriptorSet == NULL) {
            result = CTS_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }

        const CtsDescriptorSetLayout setLayout = pAllocateInfo->setLayouts[i];

        descriptorSet->bindingCount = setLayout->bindingCount;
        descriptorSet->bindings = ctsAllocation(
            &pAllocateInfo->descriptorPool->linearAllocator,
            sizeof(CtsGlDescriptorSetLayoutBinding) * descriptorSet->bindingCount,
            alignof(CtsGlDescriptorSetLayoutBinding),
            CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
        );

        for (uint32_t j = 0; j < descriptorSet->bindingCount; ++j) {
            CtsDescriptorSetLayoutBinding* source = &setLayout->bindings[j];
            CtsGlDescriptorSetLayoutBinding* target = &descriptorSet->bindings[j];
            
            target->stageFlags = source->stageFlags;
            target->binding = source->binding;
            target->descriptorCount = source->descriptorCount;
            target->descriptorType = source->descriptorType;
            target->type = parseDescriptorType(source->descriptorType);
            target->imageInfo = NULL;
            target->bufferInfo = NULL;
            target->texelBufferView = NULL;

            if (target->type == CTS_GL_DESCRIPTOR_TYPE_IMAGE_INFO) {
                target->imageInfo = ctsAllocation(
                    &pAllocateInfo->descriptorPool->linearAllocator,
                    sizeof(CtsDescriptorImageInfo) * target->descriptorCount,
                    alignof(CtsDescriptorImageInfo),
                    CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
                );
            } else if (target->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_INFO) {
                target->bufferInfo = ctsAllocation(
                    &pAllocateInfo->descriptorPool->linearAllocator,
                    sizeof(CtsDescriptorBufferInfo) * target->descriptorCount,
                    alignof(CtsDescriptorBufferInfo),
                    CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
                );
            } else if (target->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_VIEW) {
                target->texelBufferView = ctsAllocation(
                    &pAllocateInfo->descriptorPool->linearAllocator,
                    sizeof(CtsBufferView) * target->descriptorCount,
                    alignof(CtsBufferView),
                    CTS_SYSTEM_ALLOCATION_SCOPE_OBJECT
                );
            }
        }

        pDescriptorSets[i] = descriptorSet;
    }

    for (; i < pAllocateInfo->descriptorSetCount; ++i) {
        pDescriptorSets[i] = NULL;
    }

    return result;
}

void ctsUpdateDescriptorSetsImpl(
    CtsDevice pDevice,
    uint32_t pDescriptorWriteCount,
    const CtsWriteDescriptorSet* pDescriptorWrites,
    uint32_t pDescriptorCopyCount,
    const CtsCopyDescriptorSet* pDescriptorCopies
) {
    (void) pDevice;

    for (uint32_t i = 0; i < pDescriptorWriteCount; ++i) {
        const CtsWriteDescriptorSet* source = &pDescriptorWrites[i];
        CtsDescriptorSet target = source->dstSet;
        CtsGlDescriptorSetLayoutBinding* binding = &target->bindings[source->dstBinding];

        if (false) {
            // Dynamic type is not supported as for now
            binding->type = parseDescriptorType(source->descriptorType);
        }

        for (uint32_t j = 0; j < binding->descriptorCount; ++j) {
            uint32_t idx = source->dstArrayElement + j;

            if (binding->type == CTS_GL_DESCRIPTOR_TYPE_IMAGE_INFO) {
                binding->imageInfo[idx] = source->imageInfo[j];
            } else if (binding->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_INFO) {
                binding->bufferInfo[idx] = source->bufferInfo[j];
            } else if (binding->type == CTS_GL_DESCRIPTOR_TYPE_BUFFER_VIEW) {
                binding->texelBufferView[idx] = source->texelBufferView[j];
            }
        }
    }

    for (uint32_t i = 0; i < pDescriptorCopyCount; ++i) {
        // TODO: Handle copy case
        const CtsCopyDescriptorSet* descriptorSet = &pDescriptorCopies[i];
    }
}

CtsResult ctsFreeDescriptorSetsImpl(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets
) {
    for (uint32_t i = 0; pDescriptorSetCount; ++i) {
        CtsDescriptorSet descriptorSet = pDescriptorSets[i];

        for (uint32_t j = 0; j < descriptorSet->bindingCount; ++j) {
            CtsGlDescriptorSetLayoutBinding* binding = &descriptorSet->bindings[j];

            if (binding->imageInfo != NULL) {
                ctsFree(&pDescriptorPool->linearAllocator, binding->imageInfo);
            }

            if (binding->bufferInfo != NULL) {
                ctsFree(&pDescriptorPool->linearAllocator, binding->bufferInfo);
            }

            if (binding->texelBufferView != NULL) {
                ctsFree(&pDescriptorPool->linearAllocator, binding->texelBufferView);
            }
        }

        ctsFree(&pDescriptorPool->linearAllocator, descriptorSet->bindings);
        ctsFree(&pDescriptorPool->linearAllocator, pDescriptorSets[i]);
    }

    return CTS_SUCCESS;
}

#ifdef __cplusplus
}
#endif