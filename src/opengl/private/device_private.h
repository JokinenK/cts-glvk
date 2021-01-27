#pragma once

#include <stdbool.h>
#include <cts/typedefs/device.h>
#include <cts/typedefs/instance.h>
#include <cts/typedefs/physical_device.h>
#include <cts/typedefs/queue.h>
#include <cts/typedefs/gl_pipeline.h>
#include <cts/typedefs/gl_render_pass.h>
#include <cts/platform_thread.h>
#include <cts/platform_mutex.h>
#include <cts/platform_condition_variable.h>

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
    CtsBuffer buffer;
    uint32_t offset;
    GLenum indexType;
} CtsGlIndexBuffer;

struct CtsDeviceImpl {
    CtsQueue queue;
    const CtsGlGraphicsPipeline* activeGraphicsPipeline;
    
    CtsGlState state;
    CtsFramebuffer activeReadFramebuffer;
    CtsFramebuffer activeWriteFramebuffer;
    CtsRenderPass activeRenderPass;
    uint32_t activeSubpassNumber;
    const CtsGlSubpassDescription* activeSubpass;

    CtsPhysicalDevice physicalDevice;
    CtsFlags dynamicStateFlags;

    CtsGlIndexBuffer activeIndexBuffer;
};

#ifdef __cplusplus
}
#endif