#pragma once

#include <cts/typedefs/bool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsPhysicalDeviceFeatures {
    CtsBool32 robustBufferAccess;
    CtsBool32 fullDrawIndexUint32;
    CtsBool32 imageCubeArray;
    CtsBool32 independentBlend;
    CtsBool32 geometryShader;
    CtsBool32 tessellationShader;
    CtsBool32 sampleRateShading;
    CtsBool32 dualSrcBlend;
    CtsBool32 logicOp;
    CtsBool32 multiDrawIndirect;
    CtsBool32 drawIndirectFirstInstance;
    CtsBool32 depthClamp;
    CtsBool32 depthBiasClamp;
    CtsBool32 fillModeNonSolid;
    CtsBool32 depthBounds;
    CtsBool32 wideLines;
    CtsBool32 largePoints;
    CtsBool32 alphaToOne;
    CtsBool32 multiViewport;
    CtsBool32 samplerAnisotropy;
    CtsBool32 textureCompressionETC2;
    CtsBool32 textureCompressionASTC_LDR;
    CtsBool32 textureCompressionBC;
    CtsBool32 occlusionQueryPrecise;
    CtsBool32 pipelineStatisticsQuery;
    CtsBool32 vertexPipelineStoresAndAtomics;
    CtsBool32 fragmentStoresAndAtomics;
    CtsBool32 shaderTessellationAndGeometryPointSize;
    CtsBool32 shaderImageGatherExtended;
    CtsBool32 shaderStorageImageExtendedFormats;
    CtsBool32 shaderStorageImageMultisample;
    CtsBool32 shaderStorageImageReadWithoutFormat;
    CtsBool32 shaderStorageImageWriteWithoutFormat;
    CtsBool32 shaderUniformBufferArrayDynamicIndexing;
    CtsBool32 shaderSampledImageArrayDynamicIndexing;
    CtsBool32 shaderStorageBufferArrayDynamicIndexing;
    CtsBool32 shaderStorageImageArrayDynamicIndexing;
    CtsBool32 shaderClipDistance;
    CtsBool32 shaderCullDistance;
    CtsBool32 shaderFloat64;
    CtsBool32 shaderInt64;
    CtsBool32 shaderInt16;
    CtsBool32 shaderResourceResidency;
    CtsBool32 shaderResourceMinLod;
    CtsBool32 sparseBinding;
    CtsBool32 sparseResidencyBuffer;
    CtsBool32 sparseResidencyImage2D;
    CtsBool32 sparseResidencyImage3D;
    CtsBool32 sparseResidency2Samples;
    CtsBool32 sparseResidency4Samples;
    CtsBool32 sparseResidency8Samples;
    CtsBool32 sparseResidency16Samples;
    CtsBool32 sparseResidencyAliased;
    CtsBool32 variableMultisampleRate;
    CtsBool32 inheritedQueries;
} CtsPhysicalDeviceFeatures;

#ifdef __cplusplus
}
#endif