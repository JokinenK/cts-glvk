#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsStencilFaceFlags;
typedef enum CtsStencilFaceFlagBits {
    CTS_STENCIL_FACE_FRONT_BIT      = 0x00000001,
    CTS_STENCIL_FACE_BACK_BIT       = 0x00000002,
    CTS_STENCIL_FACE_FRONT_AND_BACK = 0x00000003
} CtsStencilFaceFlagBits;

#ifdef __cplusplus
}
#endif

