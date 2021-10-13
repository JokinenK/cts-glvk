#pragma once

#include <stdbool.h>
#include "vulkan/vulkan_core.h"
#include "cts/typedefs/gl_pipeline.h"
#include "cts/typedefs/gl_render_pass.h"
#include "cts/platform/platform_thread.h"
#include "cts/platform/platform_mutex.h"
#include "cts/platform/platform_condition_variable.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CtsGlTextureBinding {
    GLenum target;
    GLuint texture;
} CtsGlTextureBinding;

typedef struct CtsGlStencilState {
    GLenum func;
    GLint ref;
    GLuint mask;
    GLuint compareMask;
} CtsGlStencilState;

typedef struct CtsGlState {
    bool depthClamp;
    bool rasterizerDiscard;
    bool cullFace;
    bool depthTest;
    bool stencilTest;
    bool colorLogicOp;
    bool blend[8];
    bool scissorTest[16];

    CtsGlTextureBinding texture[32];
    GLuint sampler[32];
    GLuint program;

    GLenum primitiveTopology;
    bool primitiveRestartEnable;

    CtsGlStencilState frontStencil;
    CtsGlStencilState backStencil;
} CtsGlState;

typedef struct CtsGlIndexBuffer {
    struct CtsBuffer* buffer;
    uint32_t offset;
    GLenum indexType;
} CtsGlIndexBuffer;

struct CtsDevice {
    struct CtsQueue* queue;
    const CtsGlGraphicsPipeline* activeGraphicsPipeline;
    
    CtsGlState state;
    struct CtsFramebuffer* activeReadFramebuffer;
    struct CtsFramebuffer* activeWriteFramebuffer;
    struct CtsRenderPass* activeRenderPass;
    uint32_t activeSubpassNumber;
    const CtsGlSubpassDescription* activeSubpass;

    struct CtsPhysicalDevice* physicalDevice;
    VkFlags dynamicStateFlags;

    CtsGlIndexBuffer activeIndexBuffer;
};

#ifdef __cplusplus
}
#endif