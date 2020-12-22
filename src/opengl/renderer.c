#include <stddef.h>
#include <stdint.h>
#include <cts/align.h>
#include <string.h>
#include <cts/allocator.h>
#include <cts/types.h>
#include <cts/commands.h>
#include <cts/bump_allocator.h>
#include <cts/renderer.h>
#include <cts/renderer_proxy.h>
#include <cts/type_mapper.h>
#include <cts/commanddefs/cmd_base.h>
#include <cts/os/semaphore.h>
#include <private/instance_private.h>
#include <private/buffer_private.h>
#include <private/command_buffer_private.h>
#include <private/command_pool_private.h>
#include <private/device_private.h>
#include <private/device_memory_private.h>
#include <private/physical_device_private.h>
#include <private/shader_private.h>
#include <private/surface_private.h>
#include <private/queue_private.h>

#pragma region PrivateTypeDefinitions

typedef struct QueueItem {
    const CtsCmdBase* cmd;
    bool* finished;
} QueueItem;

static void* createCommand(
    CtsCommandBuffer pCommandBuffer,
    CtsCommandType pCommandType,
    size_t pSize,
    size_t pAlign
);

static void workerDispatch(CtsDevice pDevice, const CtsCmdBase* pCmd, bool pWait);
static void workerEntry(void* pArgs);

#pragma endregion
#pragma region PublicMethodBodies

CtsResult ctsCreateInstance(
    const CtsInstanceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsInstance* pInstance
) {
    CtsInstance instance = ctsAllocation(
        pAllocator,
        sizeof(struct CtsInstance),
        alignof(struct CtsInstance),
        CTS_SYSTEM_ALLOCATION_SCOPE_INSTANCE
    ); 

    if (instance == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    CtsPhysicalDevice physicalDevice = &instance->physicalDevice;
    physicalDevice->instance = instance;

    CtsQueueFamilyProperties* queueFamilyProperties = &instance->queueFamilyProperties;
    queueFamilyProperties->queueCount = 1;
    queueFamilyProperties->timestampValidBits = 1;
    queueFamilyProperties->minImageTransferGranularity.width = 0;
    queueFamilyProperties->minImageTransferGranularity.height = 0;
    queueFamilyProperties->minImageTransferGranularity.depth = 0;
    queueFamilyProperties->queueFlags = 
        CTS_QUEUE_GRAPHICS_BIT | 
        CTS_QUEUE_COMPUTE_BIT  | 
        CTS_QUEUE_TRANSFER_BIT | 
        CTS_QUEUE_SPARSE_BINDING_BIT; 

    instance->surface = NULL;
    *pInstance = instance;
    return CTS_SUCCESS;
}

CtsResult ctsDestroyInstance(
    CtsInstance pInstance,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pInstance != NULL) {
        ctsFree(pAllocator, pInstance);
    }

    return CTS_SUCCESS;
}

CtsResult ctsEnumeratePhysicalDevices(
    CtsInstance pInstance,
    uint32_t* pPhysicalDeviceCount,
    CtsPhysicalDevice* pPhysicalDevices
) {
    if (pPhysicalDeviceCount != NULL) {
        *pPhysicalDeviceCount = 1;
    }

    if (pPhysicalDevices != NULL) {
        *pPhysicalDevices = &pInstance->physicalDevice;
    }

    return CTS_SUCCESS;
}


CtsResult ctsGetPhysicalDeviceQueueFamilyProperties(
    CtsPhysicalDevice pPhysicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    CtsQueueFamilyProperties* pQueueFamilyProperties
) {
    if (pQueueFamilyPropertyCount != NULL) {
        *pQueueFamilyPropertyCount = 1;
    }

    if (pQueueFamilyProperties != NULL) {
        *pQueueFamilyProperties = pPhysicalDevice->instance->queueFamilyProperties;
    }

    return CTS_SUCCESS;
}

CtsResult ctsCreateDevice(
    CtsPhysicalDevice pPhysicalDevice,
    const CtsDeviceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDevice* pDevice
) {
    CtsDevice device = ctsAllocation(
        pAllocator,
        sizeof(struct CtsDevice),
        alignof(struct CtsDevice),
        CTS_SYSTEM_ALLOCATION_SCOPE_DEVICE
    );

    if (device == NULL) {
        return CTS_ERROR_OUT_OF_HOST_MEMORY;
    }

    device->isRunning = true;
    device->activeDynamicState = 0;
    device->activeIndexBuffer = NULL;
    device->activeVertexInputState = NULL;
    device->activeInputAssemblyState = NULL;
    device->activeTessellationState = NULL;
    device->activeViewportState = NULL;
    device->activeRasterizationState = NULL;
    device->activeMultisampleState = NULL;
    device->activeDepthStencilState = NULL;
    device->activeColorBlendState = NULL;
    device->physicalDevice = pPhysicalDevice;

    CtsQueueCreateInfo queueCreateInfo;
    queueCreateInfo.device = device;
    queueCreateInfo.size = 32;
    queueCreateInfo.objSize = sizeof(QueueItem);
    ctsCreateQueue(&queueCreateInfo, pAllocator, 1, &device->queue);

    CtsMutexCreateInfo mutexCreateInfo;
    ctsCreateMutexes(&mutexCreateInfo, pAllocator, 1, &device->mutex);

    CtsConditionVariableCreateInfo conditionVariableCreateInfo;
    ctsCreateConditionVariables(&conditionVariableCreateInfo, pAllocator, 1, &device->queueConditionVariable);
    ctsCreateConditionVariables(&conditionVariableCreateInfo, pAllocator, 1, &device->threadConditionVariable);

    CtsThreadCreateInfo threadCreateInfo;
    threadCreateInfo.entryPoint = workerEntry;
    threadCreateInfo.args = device;
    ctsCreateThreads(&threadCreateInfo, pAllocator, 1, &device->thread);

    *pDevice = device;
    return CTS_SUCCESS;
}

void ctsGetDeviceQueue(
    CtsDevice pDevice,
    uint32_t pQueueFamilyIndex,
    uint32_t pQueueIndex,
    CtsQueue* pQueue
) {
    (void) pQueueFamilyIndex;
    (void) pQueueIndex;

    if (pDevice != NULL) {
        *pQueue = pDevice->queue;
    }
}

void ctsDestroyDevice(
    CtsDevice pDevice,
    const CtsAllocationCallbacks* pAllocator
) {
    if (pDevice) {
        pDevice->isRunning = false;

        ctsConditionVariableWakeAll(pDevice->threadConditionVariable);
        ctsConditionVariableWakeAll(pDevice->queueConditionVariable);
        ctsDestroyThread(pDevice->thread, pAllocator);

        ctsDestroyConditionVariable(pDevice->threadConditionVariable, pAllocator);
        ctsDestroyConditionVariable(pDevice->queueConditionVariable, pAllocator);
        ctsDestroyMutex(pDevice->mutex, pAllocator);

        ctsDestroyQueue(pDevice->queue, pAllocator);

        ctsFree(pAllocator, pDevice);
    }
}

CtsResult ctsAllocateMemory(
    CtsDevice pDevice,
    const CtsMemoryAllocateInfo* pAllocateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDeviceMemory* pMemory
) {
    CtsResult result;
    CtsAllocateMemory cmd;
    cmd.base.type = CTS_COMMAND_ALLOCATE_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.allocateInfo = pAllocateInfo;
    cmd.allocator = pAllocator;
    cmd.memory = pMemory;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

CtsResult ctsMapMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset,
    CtsDeviceSize pSize,
    CtsMemoryMapFlags pFlags,
    void** pData
) {
    CtsResult result;
    CtsMapMemory cmd;
    cmd.base.type = CTS_COMMAND_MAP_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.memory = pMemory;
    cmd.offset = pOffset;
    cmd.size = pSize;
    cmd.flags = pFlags;
    cmd.data = pData;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsUnmapMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory
) {
    CtsMapMemory cmd;
    cmd.base.type = CTS_COMMAND_UNMAP_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.memory = pMemory;

    workerDispatch(pDevice, &cmd.base, true);
}

void ctsFreeMemory(
    CtsDevice pDevice,
    CtsDeviceMemory pMemory,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsFreeMemory cmd;
    cmd.base.type = CTS_COMMAND_FREE_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.memory = pMemory;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsCreateCommandPool(
    CtsDevice pDevice,
    const CtsCommandPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsCommandPool* pCommandPool
) {
    CtsResult result;
    CtsCreateCommandPool cmd;
    cmd.base.type = CTS_COMMAND_CREATE_COMMAND_POOL;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.commandPool = pCommandPool;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsDestroyCommandPool(
    CtsDevice pDevice,
    CtsCommandPool pCommandPool,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyCommandPool cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_COMMAND_POOL;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.commandPool = pCommandPool;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsCreateDescriptorPool(
    CtsDevice pDevice,
    const CtsDescriptorPoolCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorPool* pDescriptorPool
) {
    CtsResult result;
    CtsCreateDescriptorPool cmd;
    cmd.base.type = CTS_COMMAND_CREATE_DESCRIPTOR_POOL;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.descriptorPool = pDescriptorPool;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsDestroyDescriptorPool(
    CtsDevice pDevice,
    CtsDescriptorPool pDescriptorPool,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyDescriptorPool cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_DESCRIPTOR_POOL;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.descriptorPool = pDescriptorPool;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsCreateDescriptorSetLayout(
    CtsDevice pDevice,
    const CtsDescriptorSetLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsDescriptorSetLayout* pDescriptorSetLayout
) {
    CtsResult result;
    CtsCreateDescriptorSetLayout cmd;
    cmd.base.type = CTS_COMMAND_CREATE_DESCRIPTOR_SET_LAYOUT;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.descriptorSetLayout = pDescriptorSetLayout;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsDestroyDescriptorSetLayout(
    CtsDevice pDevice,
    CtsDescriptorSetLayout pDescriptorSetLayout,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyDescriptorSetLayout cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_DESCRIPTOR_SET_LAYOUT;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.descriptorSetLayout = pDescriptorSetLayout;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsCreatePipelineLayout(
    CtsDevice pDevice,
    const CtsPipelineLayoutCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipelineLayout* pPipelineLayout
) {
    CtsResult result;
    CtsCreatePipelineLayout cmd;
    cmd.base.type = CTS_COMMAND_CREATE_PIPELINE_LAYOUT;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.pipelineLayout = pPipelineLayout;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsDestroyPipelineLayout(
    CtsDevice pDevice,
    CtsPipelineLayout pPipelineLayout,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyPipelineLayout cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_PIPELINE_LAYOUT;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.pipelineLayout = pPipelineLayout;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsCreateGraphicsPipelines(
    CtsDevice pDevice,
    CtsPipelineCache pPipelineCache,
    uint32_t pCreateInfoCount,
    const CtsGraphicsPipelineCreateInfo* pCreateInfos,
    const CtsAllocationCallbacks* pAllocator,
    CtsPipeline* pPipelines
) {
    CtsResult result;
    CtsCreateGraphicsPipelines cmd;
    cmd.base.type = CTS_COMMAND_CREATE_GRAPHICS_PIPELINES;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.pipelineCache = pPipelineCache;
    cmd.createInfoCount = pCreateInfoCount;
    cmd.createInfos = pCreateInfos;
    cmd.allocator = pAllocator;
    cmd.pipelines = pPipelines;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsDestroyPipeline(
    CtsDevice pDevice,
    CtsPipeline pPipeline,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyPipeline cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_PIPELINE_LAYOUT;
    cmd.base.next = NULL;
    cmd.device = pDevice;
    cmd.pipeline = pPipeline;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsCreateBuffer(
    CtsDevice pDevice,
    const CtsBufferCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsBuffer* pBuffer
) {
    CtsResult result;
    CtsCreateBuffer cmd;
    cmd.base.type = CTS_COMMAND_CREATE_BUFFER;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.buffer = pBuffer;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsDestroyBuffer(
    CtsDevice pDevice,
    CtsBuffer pBuffer,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyBuffer cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_BUFFER;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.buffer = pBuffer;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsBindBufferMemory(
    CtsDevice pDevice,
    CtsBuffer pBuffer, 
    CtsDeviceMemory pMemory,
    CtsDeviceSize pOffset
) {
    CtsResult result;
    CtsBindBufferMemory cmd;
    cmd.base.type = CTS_COMMAND_BIND_BUFFER_MEMORY;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.buffer = pBuffer;
    cmd.memory = pMemory;
    cmd.offset = pOffset;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

CtsResult ctsCreateShaderModule(
    CtsDevice pDevice,
    const CtsShaderModuleCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsShaderModule* pShaderModule
) {
    CtsResult result;
    CtsCreateShaderModule cmd;
    cmd.base.type = CTS_COMMAND_CREATE_SHADER_MODULE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.shaderModule = pShaderModule;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsDestroyShaderModule(
    CtsDevice pDevice,
    CtsShaderModule pShaderModule,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyShaderModule cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_SHADER_MODULE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.shaderModule = pShaderModule;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsCreateFence(
    CtsDevice pDevice,
    const CtsFenceCreateInfo* pCreateInfo,
    const CtsAllocationCallbacks* pAllocator,
    CtsFence* pFence
) {
    CtsResult result;
    CtsCreateFence cmd;
    cmd.base.type = CTS_COMMAND_CREATE_FENCE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.createInfo = pCreateInfo;
    cmd.allocator = pAllocator;
    cmd.fence = pFence;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsDestroyFence(
    CtsDevice pDevice,
    CtsFence pFence,
    const CtsAllocationCallbacks* pAllocator
) {
    CtsDestroyFence cmd;
    cmd.base.type = CTS_COMMAND_DESTROY_FENCE;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.fence = pFence;
    cmd.allocator = pAllocator;

    workerDispatch(pDevice, &cmd.base, true);
}

CtsResult ctsAllocateCommandBuffers(
    CtsDevice pDevice,
    const CtsCommandBufferAllocateInfo* pAllocateInfo,
    CtsCommandBuffer* pCommandBuffers
) {
    CtsResult result;
    CtsAllocateCommandBuffers cmd;
    cmd.base.type = CTS_COMMAND_ALLOCATE_COMMAND_BUFFERS;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.allocateInfo = pAllocateInfo;
    cmd.commandBuffers = pCommandBuffers;
    cmd.result = &result;

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

void ctsFreeCommandBuffers(
    CtsDevice pDevice,
    CtsCommandPool pCommandPool,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    CtsFreeCommandBuffers cmd;
    cmd.base.type = CTS_COMMAND_FREE_COMMAND_BUFFERS;
    cmd.base.next = NULL;

    cmd.device = pDevice;
    cmd.commandPool = pCommandPool;
    cmd.commandBufferCount = pCommandBufferCount;
    cmd.commandBuffers = pCommandBuffers;

    workerDispatch(pDevice, &cmd.base, true);
}

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

    workerDispatch(pDevice, &cmd.base, true);
    return result;
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

    workerDispatch(pDevice, &cmd.base, true);
    return result;
}

CtsResult ctsBeginCommandBuffer(
    CtsCommandBuffer pCommandBuffer,
    const CtsCommandBufferBeginInfo* pBeginInfo
) {
    CtsResult result;
    CtsBeginCommandBuffer cmd;
    cmd.base.type = CTS_COMMAND_BEGIN_COMMAND_BUFFER;
    cmd.base.next = NULL;

    cmd.commandBuffer = pCommandBuffer;
    cmd.beginInfo = pBeginInfo;
    cmd.result = &result;

    workerDispatch(pCommandBuffer->device, &cmd.base, true);
    return result;
}

CtsResult ctsResetCommandBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsCommandBufferResetFlags pFlags
) {
    CtsResult result;
    CtsResetCommandBuffer cmd;
    cmd.base.type = CTS_COMMAND_RESET_COMMAND_BUFFER;
    cmd.base.next = NULL;

    cmd.commandBuffer = pCommandBuffer;
    cmd.flags = pFlags;
    cmd.result = &result;

    workerDispatch(pCommandBuffer->device, &cmd.base, true);
    return result;
}

CtsResult ctsEndCommandBuffer(
    CtsCommandBuffer pCommandBuffer
) {
    CtsResult result;
    CtsEndCommandBuffer cmd;
    cmd.base.type = CTS_COMMAND_END_COMMAND_BUFFER;
    cmd.base.next = NULL;

    cmd.commandBuffer = pCommandBuffer;
    cmd.result = &result;

    workerDispatch(pCommandBuffer->device, &cmd.base, true);
    return result;
}

CtsResult ctsQueueSubmit(
    CtsQueue pQueue,
    uint32_t pSubmitCount,
    const CtsSubmitInfo* pSubmits,
    CtsFence pFence
) {
    CtsResult result;
    CtsQueueSubmit cmd;
    cmd.base.type = CTS_COMMAND_QUEUE_SUBMIT;
    cmd.base.next = NULL;

    cmd.queue = pQueue,
    cmd.submitCount = pSubmitCount;
    cmd.submits = pSubmits;
    cmd.fence = pFence;
    cmd.result = &result;

    workerDispatch(pQueue->device, &cmd.base, true);
    return result;
}

void ctsCmdBeginQuery(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery,
    CtsQueryControlFlags pFlags
) {
    CtsCmdBeginQuery* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BEGIN_QUERY,
        sizeof(CtsCmdBeginQuery),
        alignof(CtsCmdBeginQuery)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->queryPool = pQueryPool;
    cmd->query = pQuery;
    cmd->flags = pFlags;
}

void ctsCmdBeginRenderPass(
    CtsCommandBuffer pCommandBuffer,
    const CtsRenderPassBeginInfo* pRenderPassBegin,
    CtsSubpassContents pContents
) {
    CtsCmdBeginRenderPass* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BEGIN_RENDER_PASS,
        sizeof(CtsCmdBeginRenderPass),
        alignof(CtsCmdBeginRenderPass)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->renderPassBegin = pRenderPassBegin;
    cmd->contents = pContents;
}

void ctsCmdBindDescriptorSets(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipelineLayout pPipelineLayout,
    uint32_t pFirstSet,
    uint32_t pDescriptorSetCount,
    const CtsDescriptorSet* pDescriptorSets,
    uint32_t pDynamicOffsetCount,
    const uint32_t* pDynamicOffsets
) {
    CtsCmdBindDescriptorSets* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BIND_DESCRIPTOR_SETS,
        sizeof(CtsCmdBindDescriptorSets),
        alignof(CtsCmdBindDescriptorSets)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->pipelineBindPoint = pPipelineBindPoint;
    cmd->firstSet = pFirstSet;
    cmd->descriptorSetCount = pDescriptorSetCount;
    cmd->descriptorSets = pDescriptorSets;
    cmd->dynamicOffsetCount = pDynamicOffsetCount;
    cmd->dynamicOffsets = pDynamicOffsets;
}

void ctsCmdBindIndexBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    CtsIndexType pIndexType
) {
    CtsCmdBindIndexBuffer* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BIND_INDEX_BUFFER,
        sizeof(CtsCmdBindIndexBuffer),
        alignof(CtsCmdBindIndexBuffer)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->buffer = pBuffer;
    cmd->offset = pOffset;
    cmd->indexType = pIndexType;
}

void ctsCmdBindPipeline(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineBindPoint pPipelineBindPoint,
    CtsPipeline pPipeline
) {
    CtsCmdBindPipeline* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BIND_PIPELINE,
        sizeof(CtsCmdBindPipeline),
        alignof(CtsCmdBindPipeline)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->pipelineBindPoint = pPipelineBindPoint;
    cmd->pipeline = pPipeline;
}

void ctsCmdBindVertexBuffers(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstBinding,
    uint32_t pBindingCount,
    const CtsBuffer* pBuffers,
    const CtsDeviceSize* pOffsets
) {
    CtsCmdBindVertexBuffers* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BIND_VERTEX_BUFFERS,
        sizeof(CtsCmdBindVertexBuffers),
        alignof(CtsCmdBindVertexBuffers)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->firstBinding = pFirstBinding;
    cmd->bindingCount = pBindingCount;
    cmd->buffers = pBuffers;
    cmd->offsets = pOffsets;
}

void ctsCmdBlitImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageBlit* pRegions,
    CtsFilter pFilter
) {
    CtsCmdBlitImage* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_BLIT_IMAGE,
        sizeof(CtsCmdBlitImage),
        alignof(CtsCmdBlitImage)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcImage = pSrcImage;
    cmd->srcImageLayout = pSrcImageLayout;
    cmd->dstImage = pDstImage;
    cmd->dstImageLayout = pDstImageLayout;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
    cmd->filter = pFilter;
}

void ctsCmdClearAttachments(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pAttachmentCount,
    const CtsClearAttachment* pAttachments,
    uint32_t pRectCount,
    const CtsClearRect* pRects
) {
    CtsCmdClearAttachments* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_CLEAR_ATTACHMENTS,
        sizeof(CtsCmdClearAttachments),
        alignof(CtsCmdClearAttachments)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->attachmentCount = pAttachmentCount;
    cmd->attachments = pAttachments;
    cmd->rectCount = pRectCount;
    cmd->rects = pRects;
}

void ctsCmdClearColorImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearColorValue* pColor,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    CtsCmdClearColorImage* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_CLEAR_COLOR_IMAGE,
        sizeof(CtsCmdClearColorImage),
        alignof(CtsCmdClearColorImage)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->image = pImage;
    cmd->imageLayout = pImageLayout;
    cmd->color = pColor;
    cmd->rangeCount = pRangeCount;
    cmd->ranges = pRanges;
}

void ctsCmdClearDepthStencilImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pImage,
    CtsImageLayout pImageLayout,
    const CtsClearDepthStencilValue* pDepthStencil,
    uint32_t pRangeCount,
    const CtsImageSubresourceRange* pRanges
) {
    CtsCmdClearDepthStencilImage* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_CLEAR_DEPTH_STENCIL_IMAGE,
        sizeof(CtsCmdClearDepthStencilImage),
        alignof(CtsCmdClearDepthStencilImage)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->image = pImage;
    cmd->imageLayout = pImageLayout;
    cmd->depthStencil = pDepthStencil;
    cmd->rangeCount = pRangeCount;
    cmd->ranges = pRanges;
}

void ctsCmdCopyBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferCopy* pRegions
) {
    CtsCmdCopyBuffer* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER,
        sizeof(CtsCmdCopyBuffer),
        alignof(CtsCmdCopyBuffer)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcBuffer = pSrcBuffer;
    cmd->dstBuffer = pDstBuffer;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdCopyBufferToImage(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pSrcBuffer,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
) {
    CtsCmdCopyBufferToImage* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_BUFFER_TO_IMAGE,
        sizeof(CtsCmdCopyBufferToImage),
        alignof(CtsCmdCopyBufferToImage)
    );
    cmd->commandBuffer = pCommandBuffer;
    cmd->srcBuffer = pSrcBuffer;
    cmd->dstImage = pDstImage;
    cmd->dstImageLayout = pDstImageLayout;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdCopyImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageCopy* pRegions
) {
    CtsCmdCopyImage* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE,
        sizeof(CtsCmdCopyImage),
        alignof(CtsCmdCopyImage)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcImage = pSrcImage;
    cmd->srcImageLayout = pSrcImageLayout;
    cmd->dstImage = pDstImage;
    cmd->dstImageLayout = pDstImageLayout;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdCopyImageToBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsBuffer pDstBuffer,
    uint32_t pRegionCount,
    const CtsBufferImageCopy* pRegions
) {
    CtsCmdCopyImageToBuffer* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_IMAGE_TO_BUFFER,
        sizeof(CtsCmdCopyImageToBuffer),
        alignof(CtsCmdCopyImageToBuffer)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcImage = pSrcImage;
    cmd->srcImageLayout = pSrcImageLayout;
    cmd->dstBuffer = pDstBuffer;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdCopyQueryPoolResults(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pStride,
    CtsQueryResultFlags pFlags
) {
    CtsCmdCopyQueryPoolResults* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_COPY_QUERY_POOL_RESULTS,
        sizeof(CtsCmdCopyQueryPoolResults),
        alignof(CtsCmdCopyQueryPoolResults)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->queryPool = pQueryPool;
    cmd->firstQuery = pFirstQuery;
    cmd->queryCount = pQueryCount;
    cmd->dstBuffer = pDstBuffer;
    cmd->dstOffset = pDstOffset;
    cmd->stride = pStride;
    cmd->flags = pFlags;
}

void ctsCmdDispatch(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pGroupCountX,
    uint32_t pGroupCountY,
    uint32_t pGroupCountZ
) {
    CtsCmdDispatch* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DISPATCH,
        sizeof(CtsCmdDispatch),
        alignof(CtsCmdDispatch)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->groupCountX = pGroupCountX;
    cmd->groupCountY = pGroupCountY;
    cmd->groupCountZ = pGroupCountZ;
}

void ctsCmdDispatchIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset
) {
    CtsCmdDispatchIndirect* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DISPATCH_INDIRECT,
        sizeof(CtsCmdDispatchIndirect),
        alignof(CtsCmdDispatchIndirect)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->buffer = pBuffer;
    cmd->offset = pOffset;
}

void ctsCmdDraw(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pVertexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstVertex,
    uint32_t pFirstInstance
) {
    CtsCmdDraw* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DRAW,
        sizeof(CtsCmdDraw),
        alignof(CtsCmdDraw)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->vertexCount = pVertexCount;
    cmd->instanceCount = pInstanceCount;
    cmd->firstVertex = pFirstVertex;
    cmd->firstInstance = pFirstInstance;
}

void ctsCmdDrawIndexed(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pIndexCount,
    uint32_t pInstanceCount,
    uint32_t pFirstIndex,
    int32_t pVertexOffset,
    uint32_t pFirstInstance
) {
    CtsCmdDrawIndexed* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED,
        sizeof(CtsCmdDrawIndexed),
        alignof(CtsCmdDrawIndexed)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->indexCount = pIndexCount;
    cmd->instanceCount = pInstanceCount;
    cmd->firstIndex = pFirstIndex;
    cmd->vertexOffset = pVertexOffset;
    cmd->firstInstance = pFirstInstance;
}

void ctsCmdDrawIndexedIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
) {
    CtsCmdDrawIndexedIndirect* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DRAW_INDEXED_INDIRECT,
        sizeof(CtsCmdDrawIndexedIndirect),
        alignof(CtsCmdDrawIndexedIndirect)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->buffer = pBuffer;
    cmd->offset = pOffset;
    cmd->drawCount = pDrawCount;
    cmd->stride = pStride;
}

void ctsCmdDrawIndirect(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pBuffer,
    CtsDeviceSize pOffset,
    uint32_t pDrawCount,
    uint32_t pStride
) {
    CtsCmdDrawIndirect* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_DRAW_INDIRECT,
        sizeof(CtsCmdDrawIndirect),
        alignof(CtsCmdDrawIndirect)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->buffer = pBuffer;
    cmd->offset = pOffset;
    cmd->drawCount = pDrawCount;
    cmd->stride = pStride;
}

void ctsCmdEndQuery(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
) {
    CtsCmdEndQuery* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_END_QUERY,
        sizeof(CtsCmdEndQuery),
        alignof(CtsCmdEndQuery)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->queryPool = pQueryPool;
    cmd->query = pQuery;
}

void ctsCmdEndRenderPass(
    CtsCommandBuffer pCommandBuffer
) {
    CtsCmdEndRenderPass* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_END_RENDER_PASS,
        sizeof(CtsCmdEndRenderPass),
        alignof(CtsCmdEndRenderPass)
    );

    cmd->commandBuffer = pCommandBuffer;
}

void ctsCmdExecuteCommands(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pCommandBufferCount,
    const CtsCommandBuffer* pCommandBuffers
) {
    CtsCmdExecuteCommands* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_EXECUTE_COMMANDS,
        sizeof(CtsCmdExecuteCommands),
        alignof(CtsCmdExecuteCommands)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->commandBufferCount = pCommandBufferCount;
    cmd->commandBuffers = pCommandBuffers;
}

void ctsCmdFillBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pSize,
    uint32_t pData
) {
    CtsCmdFillBuffer* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_FILL_BUFFER,
        sizeof(CtsCmdFillBuffer),
        alignof(CtsCmdFillBuffer)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->dstBuffer = pDstBuffer;
    cmd->dstOffset = pDstOffset;
    cmd->size = pSize;
    cmd->data = pData;
}

void ctsCmdNextSubpass(
    CtsCommandBuffer pCommandBuffer,
    CtsSubpassContents pContents
) {
    CtsCmdNextSubpass* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_NEXT_SUBPASS,
        sizeof(CtsCmdNextSubpass),
        alignof(CtsCmdNextSubpass)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->contents = pContents;
}

void ctsCmdPipelineBarrier(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineStageFlags pSrcStageMask,
    CtsPipelineStageFlags pDstStageMask,
    CtsDependencyFlags pDependencyFlags,
    uint32_t pMemoryBarrierCount,
    const CtsMemoryBarrier* pMemoryBarriers,
    uint32_t pBufferMemoryBarrierCount,
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t pImageMemoryBarrierCount,
    const CtsImageMemoryBarrier* pImageMemoryBarriers
) {
    CtsCmdPipelineBarrier* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_PIPELINE_BARRIER,
        sizeof(CtsCmdPipelineBarrier),
        alignof(CtsCmdPipelineBarrier)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcStageMask = pSrcStageMask;
    cmd->dstStageMask = pDstStageMask;
    cmd->dependencyFlags = pDependencyFlags;
    cmd->memoryBarrierCount = pMemoryBarrierCount;
    cmd->memoryBarriers = pMemoryBarriers;
    cmd->bufferMemoryBarrierCount = pBufferMemoryBarrierCount;
    cmd->bufferMemoryBarriers = pBufferMemoryBarriers;
    cmd->imageMemoryBarrierCount = pImageMemoryBarrierCount;
    cmd->imageMemoryBarriers = pImageMemoryBarriers;
}

void ctsCmdPushConstants(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineLayout pLayout,
    CtsShaderStageFlags pStageFlags,
    uint32_t pOffset,
    uint32_t pSize,
    const void* pValues
) {
    CtsCmdPushConstants* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_PUSH_CONSTANTS,
        sizeof(CtsCmdPushConstants),
        alignof(CtsCmdPushConstants)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->layout = pLayout;
    cmd->stageFlags = pStageFlags;
    cmd->offset = pOffset;
    cmd->size = pSize;
    cmd->values = pValues;
}

void ctsCmdResetEvent(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
) {
    CtsCmdResetEvent* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_RESET_EVENT,
        sizeof(CtsCmdResetEvent),
        alignof(CtsCmdResetEvent)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->event = pEvent;
    cmd->stageMask = pStageMask;
}

void ctsCmdResetQueryPool(
    CtsCommandBuffer pCommandBuffer,
    CtsQueryPool pQueryPool,
    uint32_t pFirstQuery,
    uint32_t pQueryCount
) {
    CtsCmdResetQueryPool* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_RESET_QUERY_POOL,
        sizeof(CtsCmdResetQueryPool),
        alignof(CtsCmdResetQueryPool)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->queryPool = pQueryPool;
    cmd->firstQuery = pFirstQuery;
    cmd->queryCount = pQueryCount;
}

void ctsCmdResolveImage(
    CtsCommandBuffer pCommandBuffer,
    CtsImage pSrcImage,
    CtsImageLayout pSrcImageLayout,
    CtsImage pDstImage,
    CtsImageLayout pDstImageLayout,
    uint32_t pRegionCount,
    const CtsImageResolve* pRegions
) {
    CtsCmdResolveImage* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_RESOLVE_IMAGE,
        sizeof(CtsCmdResolveImage),
        alignof(CtsCmdResolveImage)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->srcImage = pSrcImage;
    cmd->srcImageLayout = pSrcImageLayout;
    cmd->dstImage = pDstImage;
    cmd->dstImageLayout = pDstImageLayout;
    cmd->regionCount = pRegionCount;
    cmd->regions = pRegions;
}

void ctsCmdSetBlendConstants(
    CtsCommandBuffer pCommandBuffer,
    const float pBlendConstants[4]
) {
    CtsCmdSetBlendConstants* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_BLEND_CONSTANTS,
        sizeof(CtsCmdSetBlendConstants),
        alignof(CtsCmdSetBlendConstants)
    );

    cmd->commandBuffer = pCommandBuffer;
    memcpy(cmd->blendConstants, pBlendConstants, sizeof(float[4]));
}

void ctsCmdSetDepthBias(
    CtsCommandBuffer pCommandBuffer,
    float pDepthBiasConstantFactor,
    float pDepthBiasClamp,
    float pDepthBiasSlopeFactor
) {
    CtsCmdSetDepthBias* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BIAS,
        sizeof(CtsCmdSetDepthBias),
        alignof(CtsCmdSetDepthBias)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->depthBiasConstantFactor = pDepthBiasConstantFactor;
    cmd->depthBiasClamp = pDepthBiasClamp;
    cmd->depthBiasSlopeFactor = pDepthBiasSlopeFactor;
}

void ctsCmdSetDepthBounds(
    CtsCommandBuffer pCommandBuffer,
    float pMinDepthBounds,
    float pMaxDepthBounds
) {
    CtsCmdSetDepthBounds* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_DEPTH_BOUNDS,
        sizeof(CtsCmdSetDepthBounds),
        alignof(CtsCmdSetDepthBounds)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->minDepthBounds = pMinDepthBounds;
    cmd->maxDepthBounds = pMaxDepthBounds;
}

void ctsCmdSetDeviceMask(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pDeviceMask
) {
    CtsCmdSetDeviceMask* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_DEVICE_MASK,
        sizeof(CtsCmdSetDeviceMask),
        alignof(CtsCmdSetDeviceMask)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->deviceMask = pDeviceMask;
}

void ctsCmdSetEvent(
    CtsCommandBuffer pCommandBuffer,
    CtsEvent pEvent,
    CtsPipelineStageFlags pStageMask
) {
    CtsCmdSetEvent* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_EVENT,
        sizeof(CtsCmdSetEvent),
        alignof(CtsCmdSetEvent)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->event = pEvent;
    cmd->stageMask = pStageMask;
}

void ctsCmdSetLineWidth(
    CtsCommandBuffer pCommandBuffer,
    float pLineWidth
) {
    CtsCmdSetLineWidth* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_LINE_WIDTH,
        sizeof(CtsCmdSetLineWidth),
        alignof(CtsCmdSetLineWidth)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->lineWidth = pLineWidth;
}

void ctsCmdSetScissor(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstScissor,
    uint32_t pScissorCount,
    const CtsRect2D* pScissors
) {
    CtsCmdSetScissor* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_SCISSOR,
        sizeof(CtsCmdSetScissor),
        alignof(CtsCmdSetScissor)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->firstScissor = pFirstScissor;
    cmd->scissorCount = pScissorCount;
    cmd->scissors = pScissors;
}

void ctsCmdSetStencilCompareMask(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pCompareMask
) {
    CtsCmdSetStencilCompareMask* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_COMPARE_MASK,
        sizeof(CtsCmdSetStencilCompareMask),
        alignof(CtsCmdSetStencilCompareMask)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->faceMask = pFaceMask;
    cmd->compareMask = pCompareMask;
}

void ctsCmdSetStencilReference(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pReference
) {
    CtsCmdSetStencilReference* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_REFERENCE,
        sizeof(CtsCmdSetStencilReference),
        alignof(CtsCmdSetStencilReference)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->faceMask = pFaceMask;
    cmd->reference = pReference;
}

void ctsCmdSetStencilWriteMask(
    CtsCommandBuffer pCommandBuffer,
    CtsStencilFaceFlags pFaceMask,
    uint32_t pWriteMask
) {
    CtsCmdSetStencilWriteMask* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_STENCIL_WRITE_MASK,
        sizeof(CtsCmdSetStencilWriteMask),
        alignof(CtsCmdSetStencilWriteMask)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->faceMask = pFaceMask;
    cmd->writeMask = pWriteMask;
}

void ctsCmdSetViewport(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pFirstViewport,
    uint32_t pViewportCount,
    const CtsViewport* pViewports
) {
    CtsCmdSetViewport* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_SET_VIEWPORT,
        sizeof(CtsCmdSetViewport),
        alignof(CtsCmdSetViewport)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->firstViewport = pFirstViewport;
    cmd->viewportCount = pViewportCount;
    cmd->viewports = pViewports;
}

void ctsCmdUpdateBuffer(
    CtsCommandBuffer pCommandBuffer,
    CtsBuffer pDstBuffer,
    CtsDeviceSize pDstOffset,
    CtsDeviceSize pDataSize,
    const void* pData
) {
    CtsCmdUpdateBuffer* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_UPDATE_BUFFER,
        sizeof(CtsCmdUpdateBuffer),
        alignof(CtsCmdUpdateBuffer)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->dstBuffer = pDstBuffer;
    cmd->dstOffset = pDstOffset;
    cmd->dataSize = pDataSize;
    cmd->data = pData;
}

void ctsCmdWaitEvents(
    CtsCommandBuffer pCommandBuffer,
    uint32_t pEventCount,
    const CtsEvent* pEvents,
    CtsPipelineStageFlags pSrcStageMask,
    CtsPipelineStageFlags pDstStageMask,
    uint32_t pMemoryBarrierCount,
    const CtsMemoryBarrier* pMemoryBarriers,
    uint32_t pBufferMemoryBarrierCount,
    const CtsBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t pImageMemoryBarrierCount,
    const CtsImageMemoryBarrier* pImageMemoryBarriers
) {
    CtsCmdWaitEvents* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_WAIT_EVENTS,
        sizeof(CtsCmdWaitEvents),
        alignof(CtsCmdWaitEvents)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->eventCount = pEventCount;
    cmd->events = pEvents;
    cmd->srcStageMask = pSrcStageMask;
    cmd->dstStageMask = pDstStageMask;
    cmd->memoryBarrierCount = pMemoryBarrierCount;
    cmd->memoryBarriers = pMemoryBarriers;
    cmd->bufferMemoryBarrierCount = pBufferMemoryBarrierCount;
    cmd->bufferMemoryBarriers = pBufferMemoryBarriers;
    cmd->imageMemoryBarrierCount = pImageMemoryBarrierCount;
    cmd->imageMemoryBarriers = pImageMemoryBarriers;
}

void ctsCmdWriteTimestamp(
    CtsCommandBuffer pCommandBuffer,
    CtsPipelineStageFlagBits pPipelineStage,
    CtsQueryPool pQueryPool,
    uint32_t pQuery
) {
    CtsCmdWriteTimestamp* cmd = createCommand(
        pCommandBuffer,
        CTS_COMMAND_CMD_WRITE_TIMESTAMP,
        sizeof(CtsCmdWriteTimestamp),
        alignof(CtsCmdWriteTimestamp)
    );

    cmd->commandBuffer = pCommandBuffer;
    cmd->pipelineStage = pPipelineStage;
    cmd->queryPool = pQueryPool;
    cmd->query = pQuery;
}

#pragma endregion
#pragma region StaticMethodBodies

static void* createCommand(
    CtsCommandBuffer pCommandBuffer,
    CtsCommandType pCommandType,
    size_t pSize,
    size_t pAlign
) {
    CtsCmdBase* cmd = ctsAllocation(
        &pCommandBuffer->pool->bumpAllocator,
        pSize,
        pAlign,
        CTS_SYSTEM_ALLOCATION_SCOPE_COMMAND
    );

    cmd->type = pCommandType;
    cmd->next = NULL;

    if (pCommandBuffer->root == NULL) {
        pCommandBuffer->root = cmd;
    } else {
        pCommandBuffer->current->next = cmd;
    }
    
    pCommandBuffer->current = cmd;
    return cmd;
}

static void workerDispatch(CtsDevice pDevice, const CtsCmdBase* pCmd, bool pWait) {
    bool finished = false;

    QueueItem queueItem;
    queueItem.cmd = pCmd;
    queueItem.finished = pWait ? &finished : NULL;

    ctsMutexLock(pDevice->mutex);
    ctsQueuePush(pDevice->queue, &queueItem, sizeof(queueItem));
    ctsConditionVariableWakeAll(pDevice->threadConditionVariable);

    while (pWait && !finished) {
        ctsConditionVariableSleep(pDevice->queueConditionVariable, pDevice->mutex);
    }

    ctsMutexUnlock(pDevice->mutex);
}

static void workerEntry(void* pArgs) {
    CtsDevice device = (CtsDevice) pArgs;
    CtsPhysicalDevice physicalDevice = device->physicalDevice;
    CtsInstance instance = physicalDevice->instance;
    
    QueueItem queueItem;
    const CtsCommandMetadata* commandMetadata;

    ctsMutexLock(device->mutex);

    while (device->isRunning && instance->surface == NULL) {
        ctsConditionVariableSleep(device->threadConditionVariable, device->mutex);
    }

    ctsMutexUnlock(device->mutex);

    ctsSurfaceMakeCurrent(instance->surface);
    while (device->isRunning) {
        ctsMutexLock(device->mutex);

        while (device->isRunning && !ctsQueuePop(device->queue, &queueItem, sizeof(queueItem))) {
            ctsConditionVariableSleep(device->threadConditionVariable, device->mutex);
        }

        ctsMutexUnlock(device->mutex);

        if (!device->isRunning) {
            break;
        }

        commandMetadata = ctsGetCommandMetadata(queueItem.cmd->type);
        commandMetadata->handler(queueItem.cmd);

        if (queueItem.finished != NULL) {
            *queueItem.finished = true;
            ctsConditionVariableWakeAll(device->queueConditionVariable);
        }
    }
}

#pragma endregion