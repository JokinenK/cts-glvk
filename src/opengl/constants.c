#include <glad/glad.h>
#include <cts/constants.h>

#ifdef __cplusplus
extern "C" {
#endif

static GLint UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;
static GLint TEXTURE_BUFFER_OFFSET_ALIGNMENT = 0;

CtsDeviceSize getUniformBufferOffsetAlignment() {
    if (UNIFORM_BUFFER_OFFSET_ALIGNMENT == 0) {
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UNIFORM_BUFFER_OFFSET_ALIGNMENT);
    }

    return (CtsDeviceSize)UNIFORM_BUFFER_OFFSET_ALIGNMENT;
}

CtsDeviceSize getTextureBufferOffsetAlignment() {
    if (TEXTURE_BUFFER_OFFSET_ALIGNMENT == 0) {
        glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &TEXTURE_BUFFER_OFFSET_ALIGNMENT);
    }

    return (CtsDeviceSize)TEXTURE_BUFFER_OFFSET_ALIGNMENT;
}

#ifdef __cplusplus
}
#endif
