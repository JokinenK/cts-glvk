#include <stddef.h>
#include <string.h>
#include <glad/glad.h>
#include <cts/constants.h>
#include <cts/instance.h>
#include <cts/mutex.h>
#include <cts/condition_variable.h>
#include <private/instance_private.h>
#include <private/physical_device_private.h>
#include <private/swapchain_private.h>
#include <private/surface_private.h>

#ifdef __cplusplus
extern "C" {
#endif

static void parseDeviceFeatures(CtsPhysicalDevice physicalDevice, CtsPhysicalDeviceFeatures* pFeatures);
static void parseDeviceProperties(CtsPhysicalDevice physicalDevice, CtsPhysicalDeviceProperties* pProperties);
static void parseDeviceLimits(CtsPhysicalDevice physicalDevice, CtsPhysicalDeviceLimits* pLimits);

CtsResult ctsGetPhysicalDeviceQueueFamilyProperties(
    CtsPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    CtsQueueFamilyProperties* pQueueFamilyProperties
) {
    if (pQueueFamilyPropertyCount != NULL) {
        *pQueueFamilyPropertyCount = 1;
    }

    if (pQueueFamilyProperties != NULL) {
        *pQueueFamilyProperties = gQueueFamilyProperties;
    }

    return CTS_SUCCESS;
}

CtsResult ctsEnumerateDeviceExtensionProperties(
    CtsPhysicalDevice physicalDevice,
    const char* pLayerName,
    uint32_t* pPropertyCount,
    CtsExtensionProperties* pProperties
) {
    static const CtsExtensionProperties* properties[] = {
        &swapchainExtensionProperties,
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

    return CTS_SUCCESS;
}

void ctsGetPhysicalDeviceProperties(
    CtsPhysicalDevice physicalDevice,
    CtsPhysicalDeviceProperties* pProperties
) {
    if (pProperties != NULL) {
        *pProperties = gPhysicalDeviceProperties;
    }
}

void ctsGetPhysicalDeviceMemoryProperties(
    CtsPhysicalDevice physicalDevice,
    CtsPhysicalDeviceMemoryProperties* pMemoryProperties
) {
    if (pMemoryProperties != NULL) {
        *pMemoryProperties = gPhysicalMemoryProperties;
    }
}

void ctsPhysicalDeviceParseFeatures(CtsPhysicalDevice physicalDevice) {
    parseDeviceProperties(physicalDevice, &gPhysicalDeviceProperties);
    parseDeviceFeatures(physicalDevice, &gPhysicalDeviceFeatures);
}

const CtsMemoryType* ctsGetMemoryType(uint32_t memoryTypeIndex)
{
    return &gPhysicalMemoryProperties.memoryTypes[memoryTypeIndex];
}

static void parseDeviceFeatures(CtsPhysicalDevice physicalDevice, CtsPhysicalDeviceFeatures* pFeatures) {
    pFeatures->robustBufferAccess                       = CTS_FALSE;
    pFeatures->fullDrawIndexUint32                      = CTS_TRUE;
    pFeatures->imageCubeArray                           = GLAD_GL_ARB_texture_cube_map_array;
    pFeatures->independentBlend                         = GLAD_GL_ARB_draw_buffers_blend;
    pFeatures->geometryShader                           = CTS_TRUE;
    pFeatures->tessellationShader                       = GLAD_GL_ARB_tessellation_shader;
    pFeatures->sampleRateShading                        = CTS_FALSE;
    pFeatures->dualSrcBlend                             = CTS_TRUE;
    pFeatures->logicOp                                  = CTS_TRUE;
    pFeatures->multiDrawIndirect                        = GLAD_GL_ARB_multi_draw_indirect;
    pFeatures->drawIndirectFirstInstance                = GLAD_GL_ARB_base_instance;
    pFeatures->depthClamp                               = CTS_TRUE;
    pFeatures->depthBiasClamp                           = CTS_FALSE;
    pFeatures->fillModeNonSolid                         = CTS_TRUE;
    pFeatures->depthBounds                              = GLAD_GL_EXT_depth_bounds_test;
    pFeatures->wideLines                                = gPhysicalDeviceProperties.limits.lineWidthRange[1] > 1.0f;
    pFeatures->largePoints                              = CTS_FALSE;
    pFeatures->alphaToOne                               = CTS_FALSE;
    pFeatures->multiViewport                            = GLAD_GL_ARB_viewport_array;
    pFeatures->samplerAnisotropy                        = GLAD_GL_ARB_texture_filter_anisotropic;
    pFeatures->textureCompressionETC2                   = CTS_FALSE;
    pFeatures->textureCompressionASTC_LDR               = CTS_FALSE;
    pFeatures->textureCompressionBC                     = CTS_FALSE;
    pFeatures->occlusionQueryPrecise                    = CTS_FALSE;
    pFeatures->pipelineStatisticsQuery                  = CTS_FALSE;
    pFeatures->vertexPipelineStoresAndAtomics           = CTS_FALSE;
    pFeatures->fragmentStoresAndAtomics                 = CTS_FALSE;
    pFeatures->shaderTessellationAndGeometryPointSize   = CTS_TRUE;
    pFeatures->shaderImageGatherExtended                = CTS_FALSE;
    pFeatures->shaderStorageImageExtendedFormats        = CTS_FALSE;
    pFeatures->shaderStorageImageMultisample            = CTS_FALSE;
    pFeatures->shaderStorageImageReadWithoutFormat      = CTS_FALSE;
    pFeatures->shaderStorageImageWriteWithoutFormat     = CTS_FALSE;
    pFeatures->shaderUniformBufferArrayDynamicIndexing  = CTS_TRUE;
    pFeatures->shaderSampledImageArrayDynamicIndexing   = CTS_TRUE;
    pFeatures->shaderStorageBufferArrayDynamicIndexing  = CTS_FALSE;
    pFeatures->shaderStorageImageArrayDynamicIndexing   = CTS_FALSE;
    pFeatures->shaderClipDistance                       = CTS_TRUE;
    pFeatures->shaderCullDistance                       = GLAD_GL_ARB_cull_distance;
    pFeatures->shaderFloat64                            = CTS_FALSE;
    pFeatures->shaderInt64                              = CTS_FALSE;
    pFeatures->shaderInt16                              = CTS_TRUE;
    pFeatures->shaderResourceResidency                  = CTS_FALSE;
    pFeatures->shaderResourceMinLod                     = CTS_TRUE;
    pFeatures->sparseBinding                            = CTS_FALSE;
    pFeatures->sparseResidencyBuffer                    = CTS_FALSE;
    pFeatures->sparseResidencyImage2D                   = CTS_FALSE;
    pFeatures->sparseResidencyImage3D                   = CTS_FALSE;
    pFeatures->sparseResidency2Samples                  = CTS_FALSE;
    pFeatures->sparseResidency4Samples                  = CTS_FALSE;
    pFeatures->sparseResidency8Samples                  = CTS_FALSE;
    pFeatures->sparseResidency16Samples                 = CTS_FALSE;
    pFeatures->sparseResidencyAliased                   = CTS_FALSE;
    pFeatures->variableMultisampleRate                  = CTS_FALSE;
    pFeatures->inheritedQueries                         = CTS_FALSE;
}

static void parseDeviceProperties(CtsPhysicalDevice physicalDevice, CtsPhysicalDeviceProperties* pProperties) {
    pProperties->apiVersion = CTS_API_VERSION_1_0;
    pProperties->driverVersion = getOpenGLVersion();
    pProperties->vendorID = getVendorID();
    pProperties->deviceType = CTS_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    strncpy(pProperties->deviceName, getRenderer(), sizeof(pProperties->deviceName));

    ctsSurfaceQueryDeviceDetails(physicalDevice->surface, pProperties->vendorID, &pProperties->deviceID, pProperties->pipelineCacheUUID);
    parseDeviceLimits(physicalDevice, &pProperties->limits);
}

static void parseDeviceLimits(CtsPhysicalDevice physicalDevice, CtsPhysicalDeviceLimits* pLimits) {
    GLint i32 = 0;
    GLint i32Array[2] = {};

    GLfloat f32 = 0.0f;
    GLfloat f32Array[2] = {};

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
    pLimits->minInterpolationOffset = INT32_MIN; // Not supported
    pLimits->maxInterpolationOffset = UINT32_MAX; // Not supported
    pLimits->subPixelInterpolationOffsetBits = 0; // Not supported
    
    pLimits->maxFramebufferWidth  = pLimits->maxViewportDimensions[0];
    pLimits->maxFramebufferHeight = pLimits->maxViewportDimensions[1];
    pLimits->maxFramebufferLayers = 1; // TODO: Should this be handled?

    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &i32);
    pLimits->framebufferColorSampleCounts = (CtsSampleCountFlagBits)i32;

    glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &i32);
    pLimits->framebufferDepthSampleCounts = (CtsSampleCountFlagBits)i32;
    pLimits->framebufferStencilSampleCounts = (CtsSampleCountFlagBits)i32;

    // Not implemented, mark as 1 sample
    pLimits->framebufferNoAttachmentsSampleCounts = CTS_SAMPLE_COUNT_1_BIT;

    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &i32);
    pLimits->maxColorAttachments = i32;

    pLimits->sampledImageColorSampleCounts = pLimits->framebufferColorSampleCounts;
    pLimits->sampledImageIntegerSampleCounts = pLimits->framebufferColorSampleCounts;
    pLimits->sampledImageDepthSampleCounts = pLimits->framebufferDepthSampleCounts;
    pLimits->sampledImageStencilSampleCounts = pLimits->framebufferStencilSampleCounts;
    
    pLimits->storageImageSampleCounts = pLimits->sampledImageColorSampleCounts;
    pLimits->maxSampleMaskWords = 0; // Unsupported
    pLimits->timestampComputeAndGraphics = CTS_FALSE; // Unsupported
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

    pLimits->strictLines = CTS_FALSE;
    pLimits->standardSampleLocations = CTS_FALSE; // Can this be queried from GL?
    pLimits->optimalBufferCopyOffsetAlignment = 0; // Can this be queried from GL?
    pLimits->optimalBufferCopyRowPitchAlignment = 0; // Can this be queried from GL?
    pLimits->nonCoherentAtomSize = 0; // Can this be queried from GL?
}

#ifdef __cplusplus
}
#endif
