#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsPipelineStageFlags;
typedef enum CtsPipelineStageFlagBits {
    CTS_PIPELINE_STAGE_TOP_OF_PIPE_BIT                      = 0x00000001,
    CTS_PIPELINE_STAGE_DRAW_INDIRECT_BIT                    = 0x00000002,
    CTS_PIPELINE_STAGE_VERTEX_INPUT_BIT                     = 0x00000004,
    CTS_PIPELINE_STAGE_VERTEX_SHADER_BIT                    = 0x00000008,
    CTS_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT      = 0x00000010,
    CTS_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT   = 0x00000020,
    CTS_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                  = 0x00000040,
    CTS_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                  = 0x00000080,
    CTS_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT             = 0x00000100,
    CTS_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT              = 0x00000200,
    CTS_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT          = 0x00000400,
    CTS_PIPELINE_STAGE_COMPUTE_SHADER_BIT                   = 0x00000800,
    CTS_PIPELINE_STAGE_TRANSFER_BIT                         = 0x00001000,
    CTS_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT                   = 0x00002000,
    CTS_PIPELINE_STAGE_HOST_BIT                             = 0x00004000,
    CTS_PIPELINE_STAGE_ALL_GRAPHICS_BIT                     = 0x00008000,
    CTS_PIPELINE_STAGE_ALL_COMMANDS_BIT                     = 0x00010000
} CtsPipelineStageFlagBits;

#ifdef __cplusplus
}
#endif