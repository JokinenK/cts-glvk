#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <float.h>
#include "glad/glad.h"
#include "cts/macros.h"
#include "cts/constants.h"
#include "cts/instance.h"
#include "cts/type_mapper.h"
#include "cts/platform/platform_mutex.h"
#include "cts/platform/platform_condition_variable.h"
#include "cts/private.h"
#include "cts/instance_private.h"
#include "cts/physical_device_private.h"
#include "cts/swapchain_private.h"
#include "cts/surface_private.h"
#include "cts/queue_private.h"
#include "cts/device_info.h"

#ifdef __cplusplus
extern "C" {
#endif


static const VkExtensionProperties gKhrSwapchainExtension = {
    .extensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    .specVersion = VK_KHR_SWAPCHAIN_SPEC_VERSION,
};

static void parseDeviceFeatures(VkPhysicalDeviceFeatures* pFeatures);
static void parseDeviceProperties(VkPhysicalDeviceProperties* pProperties);
static void parseDeviceLimits(VkPhysicalDeviceLimits* pLimits);

static bool isValidFormat(VkFormat format);
static bool isPackedFormat(VkFormat format);
static bool isSignedIntegerFormat(VkFormat format);
static bool isUnsignedIntegerFormat(VkFormat format);
static bool isSignedFloatFormat(VkFormat format);
static bool isBufferCompatible(VkFormat format);
static bool hasColorComponent(VkFormat format);
static bool hasDepthComponent(VkFormat format);
static bool hasStencilComponent(VkFormat format);

void ctsPhysicalDeviceInit(struct CtsPhysicalDevice* physicalDevice, struct CtsInstance* instance, const VkAllocationCallbacks* pAllocator) {
    ctsInitPlatformMutex(&physicalDevice->mutex);
    ctsInitPlatformConditionVariable(&physicalDevice->conditionVariable);

    CtsQueueCreateInfo queueCreateInfo;
    queueCreateInfo.physicalDevice = physicalDevice;
    queueCreateInfo.size = 128;
    
    physicalDevice->instance = instance;
    physicalDevice->isRunning = true;

    ctsInitGlContext(&physicalDevice->context);
    ctsGlContextActivate(&physicalDevice->context);
    ctsPhysicalDeviceParseFeatures(physicalDevice);
    ctsGlContextDeactivate(&physicalDevice->context);

    ctsCreateQueue(&queueCreateInfo, pAllocator, &physicalDevice->queue);
}

void ctsPhysicalDeviceDestroy(struct CtsPhysicalDevice* physicalDevice, const VkAllocationCallbacks* pAllocator) {
    physicalDevice->isRunning = false;
    
    ctsWakeAllPlatformConditionVariable(&physicalDevice->conditionVariable);
    ctsDestroyPlatformConditionVariable(&physicalDevice->conditionVariable);
    ctsDestroyPlatformMutex(&physicalDevice->mutex);
    ctsDestroyQueue(physicalDevice->queue, pAllocator);
    ctsDestroyGlContext(&physicalDevice->context);
}

VkResult VKAPI_CALL ctsGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDeviceHandle,
    uint32_t* pQueueFamilyPropertyCount,
    VkQueueFamilyProperties* pQueueFamilyProperties
) {
    (void) physicalDeviceHandle;

    if (pQueueFamilyPropertyCount != NULL) {
        *pQueueFamilyPropertyCount = 1;
    }

    if (pQueueFamilyProperties != NULL) {
        *pQueueFamilyProperties = gQueueFamilyProperties;
    }

    return VK_SUCCESS;
}

VkResult VKAPI_CALL ctsEnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDeviceHandle,
    const char* pLayerName,
    uint32_t* pPropertyCount,
    VkExtensionProperties* pProperties
) {
    (void) physicalDeviceHandle;

    static const VkExtensionProperties* properties[] = {
        &gKhrSwapchainExtension,
    };

    static uint32_t propertyCount = CTS_ARRAY_SIZE(properties);

    if (pPropertyCount != NULL) {
        *pPropertyCount = propertyCount;
    }

    if (pProperties != NULL) {
        for (uint32_t i = 0; i < propertyCount; ++i) {
            pProperties[i] = *properties[i];
        }
    }

    return VK_SUCCESS;
}

void VKAPI_CALL ctsGetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkImageType type,
    VkSampleCountFlagBits samples,
    VkImageUsageFlags usage,
    VkImageTiling tiling,
    uint32_t* pPropertyCount,
    VkSparseImageFormatProperties* pProperties
) {
    if (pPropertyCount != NULL) {
        *pPropertyCount = 0;
    }
}

void VKAPI_CALL ctsGetPhysicalDeviceProperties(
    VkPhysicalDevice physicalDeviceHandle,
    VkPhysicalDeviceProperties* pProperties
) {
    if (pProperties != NULL) {
        *pProperties = gPhysicalDeviceProperties;
    }
}

void VKAPI_CALL ctsGetPhysicalDeviceFeatures(
    VkPhysicalDevice physicalDeviceHandle,
    VkPhysicalDeviceFeatures* pFeatures
) {
    if (pFeatures != NULL) {
        *pFeatures = gPhysicalDeviceFeatures;
    }
}


void VKAPI_CALL ctsGetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice physicalDeviceHandle,
    VkPhysicalDeviceMemoryProperties* pMemoryProperties
) {
    if (pMemoryProperties != NULL) {
        *pMemoryProperties = gPhysicalMemoryProperties;
    }
}

void VKAPI_CALL ctsGetPhysicalDeviceFormatProperties(
    VkPhysicalDevice physicalDeviceHandle,
    VkFormat format,
    VkFormatProperties* pFormatProperties
) {
    /*
    CTS_FORMAT_FEATURE_SAMPLED_IMAGE_BIT                = 0x00000001,
    CTS_FORMAT_FEATURE_STORAGE_IMAGE_BIT                = 0x00000002,
    CTS_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT         = 0x00000004,
    CTS_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT         = 0x00000008,
    CTS_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT         = 0x00000010,
    CTS_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT  = 0x00000020,
    CTS_FORMAT_FEATURE_VERTEX_BUFFER_BIT                = 0x00000040,
    CTS_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT             = 0x00000080,
    CTS_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT       = 0x00000100,
    CTS_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT     = 0x00000200,
    CTS_FORMAT_FEATURE_BLIT_SRC_BIT                     = 0x00000400,
    CTS_FORMAT_FEATURE_BLIT_DST_BIT                     = 0x00000800,
    CTS_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT  = 0x00001000,
    */

    pFormatProperties->bufferFeatures = 0;
    pFormatProperties->linearTilingFeatures = 0;
    pFormatProperties->optimalTilingFeatures = 0;

    if (hasColorComponent(format)) {
        pFormatProperties->linearTilingFeatures |= 
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | 
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | 
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | 
            VK_FORMAT_FEATURE_BLIT_SRC_BIT | 
            VK_FORMAT_FEATURE_BLIT_DST_BIT | 
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

        pFormatProperties->optimalTilingFeatures |= 
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | 
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | 
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | 
            VK_FORMAT_FEATURE_BLIT_SRC_BIT | 
            VK_FORMAT_FEATURE_BLIT_DST_BIT | 
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    }

    if (hasDepthComponent(format) || hasStencilComponent(format)) {
        pFormatProperties->linearTilingFeatures |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        pFormatProperties->optimalTilingFeatures |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    if (isSignedIntegerFormat(format) || isUnsignedIntegerFormat(format) || isSignedFloatFormat(format)) {
        pFormatProperties->bufferFeatures |= 
            VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT | 
            VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT | 
            VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT;
    }
}

VkResult VKAPI_CALL ctsGetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkImageType type,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImageCreateFlags flags,
    VkImageFormatProperties* pImageFormatProperties
) {
    pImageFormatProperties->maxExtent.width = 4096;
    pImageFormatProperties->maxExtent.height = 4096;
    pImageFormatProperties->maxExtent.depth = 4096;
    pImageFormatProperties->maxResourceSize = UINT32_MAX;
    pImageFormatProperties->maxArrayLayers = 16;
    pImageFormatProperties->maxMipLevels = 16;
    pImageFormatProperties->sampleCounts = (
        VK_SAMPLE_COUNT_1_BIT |
        VK_SAMPLE_COUNT_2_BIT |
        VK_SAMPLE_COUNT_4_BIT |
        VK_SAMPLE_COUNT_8_BIT |
        VK_SAMPLE_COUNT_16_BIT |
        VK_SAMPLE_COUNT_32_BIT
    );
    return VK_SUCCESS;
}

void ctsPhysicalDeviceParseFeatures() {
    parseDeviceProperties(&gPhysicalDeviceProperties);
    parseDeviceFeatures(&gPhysicalDeviceFeatures);
}

const VkMemoryType* ctsGetMemoryType(uint32_t memoryTypeIndex)
{
    return &gPhysicalMemoryProperties.memoryTypes[memoryTypeIndex];
}

static void parseDeviceFeatures(VkPhysicalDeviceFeatures* pFeatures) {
    pFeatures->robustBufferAccess                       = VK_FALSE;
    pFeatures->fullDrawIndexUint32                      = VK_TRUE;
    pFeatures->imageCubeArray                           = GLAD_GL_ARB_texture_cube_map_array;
    pFeatures->independentBlend                         = GLAD_GL_ARB_draw_buffers_blend;
    pFeatures->geometryShader                           = VK_TRUE;
    pFeatures->tessellationShader                       = GLAD_GL_ARB_tessellation_shader;
    pFeatures->sampleRateShading                        = GLAD_GL_ARB_sample_shading;
    pFeatures->dualSrcBlend                             = VK_TRUE;
    pFeatures->logicOp                                  = VK_TRUE;
    pFeatures->multiDrawIndirect                        = GLAD_GL_ARB_multi_draw_indirect;
    pFeatures->drawIndirectFirstInstance                = GLAD_GL_ARB_base_instance;
    pFeatures->depthClamp                               = VK_TRUE;
    pFeatures->depthBiasClamp                           = VK_FALSE;
    pFeatures->fillModeNonSolid                         = VK_TRUE;
    pFeatures->depthBounds                              = GLAD_GL_EXT_depth_bounds_test;
    pFeatures->wideLines                                = gPhysicalDeviceProperties.limits.lineWidthRange[1] > 1.0f;
    pFeatures->largePoints                              = VK_FALSE;
    pFeatures->alphaToOne                               = VK_FALSE;
    pFeatures->multiViewport                            = GLAD_GL_ARB_viewport_array;
    pFeatures->samplerAnisotropy                        = GLAD_GL_ARB_texture_filter_anisotropic;
    pFeatures->textureCompressionETC2                   = VK_FALSE;
    pFeatures->textureCompressionASTC_LDR               = VK_FALSE;
    pFeatures->textureCompressionBC                     = VK_FALSE;
    pFeatures->occlusionQueryPrecise                    = VK_FALSE;
    pFeatures->pipelineStatisticsQuery                  = VK_FALSE;
    pFeatures->vertexPipelineStoresAndAtomics           = VK_FALSE;
    pFeatures->fragmentStoresAndAtomics                 = VK_FALSE;
    pFeatures->shaderTessellationAndGeometryPointSize   = VK_TRUE;
    pFeatures->shaderImageGatherExtended                = VK_FALSE;
    pFeatures->shaderStorageImageExtendedFormats        = VK_FALSE;
    pFeatures->shaderStorageImageMultisample            = VK_FALSE;
    pFeatures->shaderStorageImageReadWithoutFormat      = VK_FALSE;
    pFeatures->shaderStorageImageWriteWithoutFormat     = VK_FALSE;
    pFeatures->shaderUniformBufferArrayDynamicIndexing  = VK_TRUE;
    pFeatures->shaderSampledImageArrayDynamicIndexing   = VK_TRUE;
    pFeatures->shaderStorageBufferArrayDynamicIndexing  = VK_FALSE;
    pFeatures->shaderStorageImageArrayDynamicIndexing   = VK_FALSE;
    pFeatures->shaderClipDistance                       = VK_TRUE;
    pFeatures->shaderCullDistance                       = GLAD_GL_ARB_cull_distance;
    pFeatures->shaderFloat64                            = VK_FALSE;
    pFeatures->shaderInt64                              = VK_FALSE;
    pFeatures->shaderInt16                              = VK_TRUE;
    pFeatures->shaderResourceResidency                  = VK_FALSE;
    pFeatures->shaderResourceMinLod                     = VK_TRUE;
    pFeatures->sparseBinding                            = VK_FALSE;
    pFeatures->sparseResidencyBuffer                    = VK_FALSE;
    pFeatures->sparseResidencyImage2D                   = VK_FALSE;
    pFeatures->sparseResidencyImage3D                   = VK_FALSE;
    pFeatures->sparseResidency2Samples                  = VK_FALSE;
    pFeatures->sparseResidency4Samples                  = VK_FALSE;
    pFeatures->sparseResidency8Samples                  = VK_FALSE;
    pFeatures->sparseResidency16Samples                 = VK_FALSE;
    pFeatures->sparseResidencyAliased                   = VK_FALSE;
    pFeatures->variableMultisampleRate                  = VK_FALSE;
    pFeatures->inheritedQueries                         = VK_FALSE;
}

static void parseDeviceProperties(VkPhysicalDeviceProperties* pProperties) {
    pProperties->apiVersion = VK_API_VERSION_1_0;
    pProperties->driverVersion = getOpenGLVersion();
    pProperties->vendorID = getVendorID();
    pProperties->deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    strncpy(pProperties->deviceName, getRenderer(), sizeof(pProperties->deviceName));

    ctsDeviceInfoQuery(pProperties->vendorID, &pProperties->deviceID, pProperties->pipelineCacheUUID);
    parseDeviceLimits(&pProperties->limits);
}

static void parseDeviceLimits(VkPhysicalDeviceLimits* pLimits) {
    GLint i32 = 0;
    GLint i32Array[2];

    GLfloat f32 = 0.0f;
    GLfloat f32Array[2];

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i32);
    pLimits->maxImageDimension1D = i32;
    pLimits->maxImageDimension2D = i32;

    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &i32);
    pLimits->maxImageDimension3D = i32;

    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &i32);
    pLimits->maxImageDimensionCube = i32;

    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &i32);
    pLimits->maxImageArrayLayers = i32;

    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &i32);
    pLimits->maxTexelBufferElements = i32;

    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &i32);
    pLimits->maxUniformBufferRange = i32;

    //glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &i32);
    pLimits->maxStorageBufferRange = 0;

    pLimits->maxPushConstantsSize = 0; // Not supported
    pLimits->maxMemoryAllocationCount = 4096; // Mimic vulkan
    pLimits->maxSamplerAllocationCount = 0xFFFF; // No way to read from GL
    pLimits->bufferImageGranularity = 1; // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#resources-bufferimagegranularity
    pLimits->sparseAddressSpaceSize = 0; // Not supported atleast for now
    pLimits->maxBoundDescriptorSets = 4; // Lower end value from gpuinfo.org

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &i32);
    pLimits->maxPerStageDescriptorSamplers = i32;

    pLimits->maxPerStageDescriptorUniformBuffers = 12; // Lower end value from gpuinfo.org
    pLimits->maxPerStageDescriptorStorageBuffers = 16; // Lower end value from gpuinfo.org
    pLimits->maxPerStageDescriptorSampledImages = 200; // Lower end value from gpuinfo.org
    pLimits->maxPerStageDescriptorStorageImages = 16; // Lower end value from gpuinfo.org
    pLimits->maxPerStageDescriptorInputAttachments = 8; // Lower end value from gpuinfo.org
    pLimits->maxPerStageResources = 200; // Lower end value from gpuinfo.org

    pLimits->maxDescriptorSetSamplers = 576; // Lower end value from gpuinfo.org
    pLimits->maxDescriptorSetUniformBuffers = 72; // Lower end value from gpuinfo.org
    pLimits->maxDescriptorSetUniformBuffersDynamic = 8; // Lower end value from gpuinfo.org
    pLimits->maxDescriptorSetStorageBuffers = 96; // Lower end value from gpuinfo.org
    pLimits->maxDescriptorSetStorageBuffersDynamic = 8; // Lower end value from gpuinfo.org
    pLimits->maxDescriptorSetSampledImages = 1200; // Lower end value from gpuinfo.org
    pLimits->maxDescriptorSetStorageImages = 196; // Lower end value from gpuinfo.org
    pLimits->maxDescriptorSetInputAttachments = 8; // Lower end value from gpuinfo.org

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &i32);
    pLimits->maxVertexInputAttributes = i32;
    
    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &i32);
    pLimits->maxVertexInputBindings = i32;

    glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &i32);
    pLimits->maxVertexInputAttributeOffset = i32;

    i32 = 2048; // glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, &i32); // https://www.khronos.org/opengl/wiki/Vertex_Specification 
    pLimits->maxVertexInputBindingStride = i32;

    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &i32);
    pLimits->maxVertexOutputComponents = i32;

    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &i32);
    pLimits->maxTessellationGenerationLevel = i32;

    glGetIntegerv(GL_MAX_PATCH_VERTICES, &i32);
    pLimits->maxTessellationPatchSize = i32;

    glGetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, &i32);
    pLimits->maxTessellationControlPerVertexInputComponents = i32;

    glGetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, &i32);
    pLimits->maxTessellationControlPerVertexOutputComponents = i32;

    glGetIntegerv(GL_MAX_TESS_PATCH_COMPONENTS, &i32);
    pLimits->maxTessellationControlPerPatchOutputComponents = i32;
    
    glGetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, &i32);
    pLimits->maxTessellationControlTotalOutputComponents = i32;

    glGetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, &i32);
    pLimits->maxTessellationEvaluationInputComponents = i32;

    glGetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, &i32);
    pLimits->maxTessellationEvaluationOutputComponents = i32;

    i32 = 1; // glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &i32); // https://www.khronos.org/opengl/wiki/Geometry_Shader
    pLimits->maxGeometryShaderInvocations = i32;
    
    glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &i32);
    pLimits->maxGeometryInputComponents = i32;

    glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &i32);
    pLimits->maxGeometryOutputComponents = i32;

    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &i32);
    pLimits->maxGeometryOutputVertices = i32;

    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &i32);
    pLimits->maxGeometryTotalOutputComponents = i32;

    glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &i32);
    pLimits->maxFragmentInputComponents = i32;

    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &i32);
    pLimits->maxFragmentOutputAttachments = i32;

    glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &i32);
    pLimits->maxFragmentDualSrcAttachments = i32;
    pLimits->maxFragmentCombinedOutputResources = i32; // https://www.khronos.org/opengl/wiki/Blending#Dual_Source_Blending

    i32 = 16384; // https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html
    pLimits->maxComputeSharedMemorySize = i32;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &i32);
    pLimits->maxComputeWorkGroupCount[0] = i32;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &i32);
    pLimits->maxComputeWorkGroupCount[1] = i32;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &i32);
    pLimits->maxComputeWorkGroupCount[2] = i32;

    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &i32);
    pLimits->maxComputeWorkGroupInvocations = i32;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &i32);
    pLimits->maxComputeWorkGroupSize[0] = i32;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &i32);
    pLimits->maxComputeWorkGroupSize[1] = i32;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &i32);
    pLimits->maxComputeWorkGroupSize[2] = i32;

    pLimits->subPixelPrecisionBits = 0; // Unsupported
    pLimits->subTexelPrecisionBits = 0; // Unsupported
    pLimits->mipmapPrecisionBits = 0; // Unsupported
    pLimits->maxDrawIndexedIndexValue = (UINT32_MAX - 1U); // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPhysicalDeviceLimits.html
    pLimits->maxDrawIndirectCount = UINT32_MAX;

    glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &f32);
    pLimits->maxSamplerLodBias = f32;

    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &f32);
    pLimits->maxSamplerAnisotropy = f32;

    glGetIntegerv(GL_MAX_VIEWPORTS, &i32);
    pLimits->maxViewports = i32;

    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, i32Array);
    pLimits->maxViewportDimensions[0] = i32Array[0];
    pLimits->maxViewportDimensions[1] = i32Array[1];

    glGetFloatv(GL_VIEWPORT_BOUNDS_RANGE, f32Array);
    pLimits->viewportBoundsRange[0] = f32Array[0];
    pLimits->viewportBoundsRange[1] = f32Array[1];

    pLimits->viewportSubPixelBits;

    glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &i32);
    pLimits->minMemoryMapAlignment = i32;

    glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &i32);
    pLimits->minTexelBufferOffsetAlignment;

    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &i32);
    pLimits->minUniformBufferOffsetAlignment = i32;

    i32 = UINT32_MAX; // glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &i32);
    pLimits->minStorageBufferOffsetAlignment = i32;
    
    glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &i32);
    pLimits->minTexelOffset = i32;

    glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &i32);
    pLimits->maxTexelOffset = i32;

    pLimits->minTexelGatherOffset = INT32_MIN; // Not supported
    pLimits->maxTexelGatherOffset = UINT32_MAX; // Not supported
    pLimits->minInterpolationOffset = FLT_MIN; // Not supported
    pLimits->maxInterpolationOffset = FLT_MAX; // Not supported
    pLimits->subPixelInterpolationOffsetBits = 0; // Not supported
    
    pLimits->maxFramebufferWidth  = pLimits->maxViewportDimensions[0];
    pLimits->maxFramebufferHeight = pLimits->maxViewportDimensions[1];
    pLimits->maxFramebufferLayers = 1; // TODO: Should this be handled?

    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &i32);
    pLimits->framebufferColorSampleCounts = (VkSampleCountFlagBits)i32;

    glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &i32);
    pLimits->framebufferDepthSampleCounts = (VkSampleCountFlagBits)i32;
    pLimits->framebufferStencilSampleCounts = (VkSampleCountFlagBits)i32;

    // Not implemented, mark as 1 sample
    pLimits->framebufferNoAttachmentsSampleCounts = VK_SAMPLE_COUNT_1_BIT;

    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &i32);
    pLimits->maxColorAttachments = i32;

    pLimits->sampledImageColorSampleCounts = pLimits->framebufferColorSampleCounts;
    pLimits->sampledImageIntegerSampleCounts = pLimits->framebufferColorSampleCounts;
    pLimits->sampledImageDepthSampleCounts = pLimits->framebufferDepthSampleCounts;
    pLimits->sampledImageStencilSampleCounts = pLimits->framebufferStencilSampleCounts;
    
    pLimits->storageImageSampleCounts = pLimits->sampledImageColorSampleCounts;
    pLimits->maxSampleMaskWords = 0; // Unsupported
    pLimits->timestampComputeAndGraphics = VK_FALSE; // Unsupported
    pLimits->timestampPeriod = 0; // Unsupported

    glGetIntegerv(GL_MAX_CLIP_DISTANCES, &i32);
    pLimits->maxClipDistances = i32;

    glGetIntegerv(GL_MAX_CULL_DISTANCES, &i32);
    pLimits->maxCullDistances = i32;

    glGetIntegerv(GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES, &i32);
    pLimits->maxCombinedClipAndCullDistances = i32;

    i32 = 2; // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPhysicalDeviceLimits.html
    pLimits->discreteQueuePriorities = i32;
    
    glGetFloatv(GL_POINT_SIZE_RANGE, f32Array);
    pLimits->pointSizeRange[0] = f32Array[0];
    pLimits->pointSizeRange[1] = f32Array[1];

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, f32Array);
    pLimits->lineWidthRange[0] = f32Array[0];
    pLimits->lineWidthRange[1] = f32Array[1];

    glGetFloatv(GL_POINT_SIZE_GRANULARITY, &f32);
    pLimits->pointSizeGranularity = f32;

    glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &f32);
    pLimits->lineWidthGranularity = f32;

    pLimits->strictLines = VK_FALSE;
    pLimits->standardSampleLocations = VK_FALSE; // Can this be queried from GL?
    pLimits->optimalBufferCopyOffsetAlignment = 0; // Can this be queried from GL?
    pLimits->optimalBufferCopyRowPitchAlignment = 0; // Can this be queried from GL?
    pLimits->nonCoherentAtomSize = 0; // Can this be queried from GL?
}

static bool isValidFormat(VkFormat format) {
    return (format != VK_FORMAT_UNDEFINED && parseFormat(format) != NULL);
}

static bool isPackedFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
        case VK_FORMAT_X8_D24_UNORM_PACK32: {
            return true;
        } break;

        default: {
            return false;
        } break;
    }
}

static bool isSignedIntegerFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R8_SINT: 
        case VK_FORMAT_R8G8_SINT: 
        case VK_FORMAT_R8G8B8_SINT: 
        case VK_FORMAT_B8G8R8_SINT: 
        case VK_FORMAT_R8G8B8A8_SINT: 
        case VK_FORMAT_B8G8R8A8_SINT: 
        case VK_FORMAT_R16_SINT: 
        case VK_FORMAT_R16G16_SINT: 
        case VK_FORMAT_R16G16B16_SINT: 
        case VK_FORMAT_R16G16B16A16_SINT: 
        case VK_FORMAT_R32_SINT: 
        case VK_FORMAT_R32G32_SINT: 
        case VK_FORMAT_R32G32B32_SINT: 
        case VK_FORMAT_R32G32B32A32_SINT: {
            return true;
        } break;

        default: {
            return false;
        } break;
    }
}

static bool isUnsignedIntegerFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R8_UINT: 
        case VK_FORMAT_R8G8_UINT: 
        case VK_FORMAT_R8G8B8_UINT: 
        case VK_FORMAT_B8G8R8_UINT: 
        case VK_FORMAT_R8G8B8A8_UINT: 
        case VK_FORMAT_B8G8R8A8_UINT: 
        case VK_FORMAT_R16_UINT: 
        case VK_FORMAT_R16G16_UINT: 
        case VK_FORMAT_R16G16B16_UINT: 
        case VK_FORMAT_R16G16B16A16_UINT: 
        case VK_FORMAT_R32_UINT: 
        case VK_FORMAT_R32G32_UINT: 
        case VK_FORMAT_R32G32B32_UINT: 
        case VK_FORMAT_R32G32B32A32_UINT: {
            return true;
        } break;

        default: {
            return false;
        } break;
    }
}

static bool isSignedFloatFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R16_SFLOAT: 
        case VK_FORMAT_R16G16_SFLOAT: 
        case VK_FORMAT_R16G16B16_SFLOAT: 
        case VK_FORMAT_R16G16B16A16_SFLOAT: 
        case VK_FORMAT_R32_SFLOAT: 
        case VK_FORMAT_R32G32_SFLOAT: 
        case VK_FORMAT_R32G32B32_SFLOAT: 
        case VK_FORMAT_R32G32B32A32_SFLOAT: {
            return true;
        } break;

        default: {
            return false;
        } break;
    }
}

static bool isBufferCompatible(VkFormat format) {
    return isSignedIntegerFormat(format) || isUnsignedIntegerFormat(format) || isSignedFloatFormat(format);
}

static bool hasColorComponent(VkFormat format) {
    return (isValidFormat(format) && !hasDepthComponent(format) && !hasStencilComponent(format));
}

static bool hasDepthComponent(VkFormat format) {
    switch (format) {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT: {
            return true;
        } break;
        
        default: {
            return false;
        } break;
    }
}

static bool hasStencilComponent(VkFormat format) {
    switch (format) {
        case VK_FORMAT_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT: {
            return true;
        } break;
        
        default: {
            return false;
        } break;
    }
}

#ifdef __cplusplus
}
#endif
