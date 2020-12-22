#pragma once

#include <glad/glad.h>
#include <cts/typedefs/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CtsImage {
    GLenum handle;
    GLenum internalFormat;
    GLenum type;
    GLenum target;
    CtsImageType imageType;
};

#ifdef __cplusplus
}
#endif
