#pragma once

#include <cts/typedefs/flags.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef CtsFlags CtsAccessFlags;
typedef enum CtsAccessFlagBits {
    CTS_ACCESS_INDIRECT_COMMAND_READ_BIT            = 0x00000001,
    CTS_ACCESS_INDEX_READ_BIT                       = 0x00000002,
    CTS_ACCESS_VERTEX_ATTRIBUTE_READ_BIT            = 0x00000004,
    CTS_ACCESS_UNIFORM_READ_BIT                     = 0x00000008,
    CTS_ACCESS_INPUT_ATTACHMENT_READ_BIT            = 0x00000010,
    CTS_ACCESS_SHADER_READ_BIT                      = 0x00000020,
    CTS_ACCESS_SHADER_WRITE_BIT                     = 0x00000040,
    CTS_ACCESS_COLOR_ATTACHMENT_READ_BIT            = 0x00000080,
    CTS_ACCESS_COLOR_ATTACHMENT_WRITE_BIT           = 0x00000100,
    CTS_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT    = 0x00000200,
    CTS_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT   = 0x00000400,
    CTS_ACCESS_TRANSFER_READ_BIT                    = 0x00000800,
    CTS_ACCESS_TRANSFER_WRITE_BIT                   = 0x00001000,
    CTS_ACCESS_HOST_READ_BIT                        = 0x00002000,
    CTS_ACCESS_HOST_WRITE_BIT                       = 0x00004000,
    CTS_ACCESS_MEMORY_READ_BIT                      = 0x00008000,
    CTS_ACCESS_MEMORY_WRITE_BIT                     = 0x00010000,
} CtsAccessFlagBits;

#ifdef __cplusplus
}
#endif