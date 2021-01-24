#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsShaderStageFlags;
typedef enum CtsShaderStageFlagBits {
    CTS_SHADER_STAGE_VERTEX_BIT                     = 0x00000001,
    CTS_SHADER_STAGE_TESSELLATION_CONTROL_BIT       = 0x00000002,
    CTS_SHADER_STAGE_TESSELLATION_EVALUATION_BIT    = 0x00000004,
    CTS_SHADER_STAGE_GEOMETRY_BIT                   = 0x00000008,
    CTS_SHADER_STAGE_FRAGMENT_BIT                   = 0x00000010,
    CTS_SHADER_STAGE_COMPUTE_BIT                    = 0x00000020,
    CTS_SHADER_STAGE_ALL_GRAPHICS                   = 0x0000001F,
    CTS_SHADER_STAGE_ALL                            = 0x7FFFFFFF
} CtsShaderStageFlagBits;

#ifdef __cplusplus
}
#endif

