#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <cts/gl_enums.h>
#include <cts/type_mapper.h>

#ifndef COUNTOF
#  define COUNTOF(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

static const float EPSILON = 1.0f / 1000.0f;

static const GLenum sCompareOps[] = {
    GL_NEVER,    // VK_COMPARE_OP_NEVER = 0,
    GL_LESS,     // VK_COMPARE_OP_LESS = 1,
    GL_EQUAL,    // VK_COMPARE_OP_EQUAL = 2,
    GL_LEQUAL,   // VK_COMPARE_OP_LESS_OR_EQUAL = 3,
    GL_GREATER,  // VK_COMPARE_OP_GREATER = 4,
    GL_NOTEQUAL, // VK_COMPARE_OP_NOT_EQUAL = 5,
    GL_GEQUAL,   // VK_COMPARE_OP_GREATER_OR_EQUAL = 6,
    GL_ALWAYS    // VK_COMPARE_OP_ALWAYS = 7,
};

static const GLenum sStencilOps[] = {
    GL_KEEP,      // VK_STENCIL_OP_KEEP = 0,
    GL_ZERO,      // VK_STENCIL_OP_ZERO = 1,
    GL_REPLACE,   // VK_STENCIL_OP_REPLACE = 2,
    GL_INCR,      // VK_STENCIL_OP_INCREMENT_AND_CLAMP = 3,
    GL_DECR,      // VK_STENCIL_OP_DECREMENT_AND_CLAMP = 4,
    GL_INVERT,    // VK_STENCIL_OP_INVERT = 5,
    GL_INCR_WRAP, // VK_STENCIL_OP_INCREMENT_AND_WRAP = 6,
    GL_DECR_WRAP  // VK_STENCIL_OP_DECREMENT_AND_WRAP = 7,
};

// These values are swapped intentionally, OpenGL renders upside down compared to Vulkan
static const GLenum sFrontFaces[] = {
    GL_CW,  // VK_FRONT_FACE_COUNTER_CLOCKWISE = 0,
    GL_CCW, // VK_FRONT_FACE_CLOCKWISE = 1,
};

static const GLenum sPrimitiveTopologies[] = {
    GL_POINTS,                    // VK_PRIMITIVE_TOPOLOGY_POINT_LIST = 0, 
    GL_LINES,                     // VK_PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
    GL_LINE_STRIP,                // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
    GL_TRIANGLES,                 // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
    GL_TRIANGLE_STRIP,            // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
    GL_TRIANGLE_FAN,              // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5,
    GL_LINES_ADJACENCY,           // VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
    GL_LINE_STRIP_ADJACENCY,      // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
    GL_TRIANGLES_ADJACENCY,       // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
    GL_TRIANGLE_STRIP_ADJACENCY,  // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
    GL_INVALID_ENUM, //GL_PATCHES // VK_PRIMITIVE_TOPOLOGY_PATCH_LIST = 10,
};

static const GLenum sPolygonModes[] = {
    GL_FILL,  // VK_POLYGON_MODE_FILL = 0,
    GL_LINE,  // VK_POLYGON_MODE_LINE = 1,
    GL_POINT, // VK_POLYGON_MODE_POINT = 2,
};

static const GLenum sSamplerAddressModes[] = {
    GL_REPEAT, // VK_SAMPLER_ADDRESS_MODE_REPEAT = 0
    GL_MIRRORED_REPEAT, // VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1
    GL_CLAMP_TO_EDGE, // VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2
    GL_CLAMP_TO_BORDER, // VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3
    GL_INVALID_ENUM, // VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = 4
};

static const GLint sMagFilters[] = {
    GL_NEAREST,
    GL_LINEAR
};

static const GLint sMinFiltersLinear[] = {
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_LINEAR
};

static const GLint sMinFiltersNearest[] = {
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR
};

static GLenum sImageTypes[] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D
};

static GLenum sImageArrayTypes[] = {
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D_ARRAY,
    GL_INVALID_ENUM
};

static GLenum sMultisampleImageTypes[] = {
    GL_INVALID_ENUM,
    GL_TEXTURE_2D_MULTISAMPLE,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY
};

static GLenum sMultisampleImageArrayTypes[] = {
    GL_INVALID_ENUM,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
    GL_INVALID_ENUM
};

static GLenum sImageViewTypes[] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_CUBE_MAP_ARRAY
};

static GLenum sMultisampleImageViewTypes[] = {
    GL_INVALID_ENUM,
    GL_TEXTURE_2D_MULTISAMPLE,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
    GL_INVALID_ENUM,
    GL_INVALID_ENUM,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
    GL_INVALID_ENUM
};

static const CtsFormatData sFormats[] = {
    { VK_FORMAT_UNDEFINED,                  GL_INVALID_ENUM,    GL_INVALID_ENUM,                   GL_INVALID_ENUM,       0, GL_FALSE },

    { VK_FORMAT_R4G4B4A4_UNORM_PACK16,      GL_RGBA,            GL_UNSIGNED_SHORT_4_4_4_4,         GL_RGBA4,              4, GL_FALSE },
    { VK_FORMAT_B4G4R4A4_UNORM_PACK16,      GL_BGRA,            GL_UNSIGNED_SHORT_4_4_4_4_REV,     GL_RGBA4,              4, GL_FALSE },

    { VK_FORMAT_R5G6B5_UNORM_PACK16,        GL_RGB,             GL_UNSIGNED_SHORT_5_6_5,           GL_RGB8,               3, GL_FALSE },
    { VK_FORMAT_B5G6R5_UNORM_PACK16,        GL_RGB,             GL_UNSIGNED_SHORT_5_6_5,           GL_RGB8,               3, GL_FALSE },

    { VK_FORMAT_R5G5B5A1_UNORM_PACK16,      GL_RGBA,            GL_UNSIGNED_SHORT_5_5_5_1,         GL_RGB5_A1,            4, GL_FALSE },
    { VK_FORMAT_B5G5R5A1_UNORM_PACK16,      GL_BGRA,            GL_UNSIGNED_SHORT_5_5_5_1,         GL_RGB5_A1,            4, GL_FALSE },
    { VK_FORMAT_A1R5G5B5_UNORM_PACK16,      GL_RGBA,            GL_UNSIGNED_SHORT_1_5_5_5_REV,     GL_RGB5_A1,            4, GL_FALSE },

    { VK_FORMAT_R8_UNORM,                   GL_RED,             GL_UNSIGNED_BYTE,                  GL_R8,                 1, GL_FALSE },
    { VK_FORMAT_R8_SNORM,                   GL_RED,             GL_BYTE,                           GL_R8_SNORM,           1, GL_FALSE },
    { VK_FORMAT_R8_USCALED,                 GL_RED,             GL_UNSIGNED_BYTE,                  GL_R8,                 1, GL_TRUE  },
    { VK_FORMAT_R8_SSCALED,                 GL_RED,             GL_BYTE,                           GL_R8_SNORM,           1, GL_TRUE  },
    { VK_FORMAT_R8_UINT,                    GL_RED_INTEGER,     GL_UNSIGNED_BYTE,                  GL_R8UI,               1, GL_FALSE },
    { VK_FORMAT_R8_SINT,                    GL_RED_INTEGER,     GL_BYTE,                           GL_R8I,                1, GL_FALSE },

    { VK_FORMAT_R8G8_UNORM,                 GL_RG,              GL_UNSIGNED_BYTE,                  GL_RG8,                2, GL_FALSE },
    { VK_FORMAT_R8G8_SNORM,                 GL_RG,              GL_BYTE,                           GL_RG8_SNORM,          2, GL_FALSE },
    { VK_FORMAT_R8G8_USCALED,               GL_RG,              GL_UNSIGNED_BYTE,                  GL_RG8,                2, GL_TRUE  },
    { VK_FORMAT_R8G8_SSCALED,               GL_RG,              GL_BYTE,                           GL_RG8_SNORM,          2, GL_TRUE  },
    { VK_FORMAT_R8G8_UINT,                  GL_RG_INTEGER,      GL_UNSIGNED_BYTE,                  GL_RG8UI,              2, GL_FALSE },
    { VK_FORMAT_R8G8_SINT,                  GL_RG_INTEGER,      GL_BYTE,                           GL_RG8I,               2, GL_FALSE },

    { VK_FORMAT_R8G8B8_UNORM,               GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RGB8,               3, GL_FALSE },
    { VK_FORMAT_R8G8B8_SNORM,               GL_RGB,             GL_BYTE,                           GL_RGB8_SNORM,         3, GL_FALSE },
    { VK_FORMAT_R8G8B8_USCALED,             GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RGB8,               3, GL_TRUE  },
    { VK_FORMAT_R8G8B8_SSCALED,             GL_RGB,             GL_BYTE,                           GL_RGB8_SNORM,         3, GL_TRUE  },
    { VK_FORMAT_R8G8B8_UINT,                GL_RGB_INTEGER,     GL_UNSIGNED_BYTE,                  GL_RGB8UI,             3, GL_FALSE },
    { VK_FORMAT_R8G8B8_SINT,                GL_RGB_INTEGER,     GL_BYTE,                           GL_RGB8I,              3, GL_FALSE },
    { VK_FORMAT_R8G8B8_SRGB,                GL_RGB,             GL_UNSIGNED_BYTE,                  GL_SRGB8,              3, GL_FALSE },

    { VK_FORMAT_B8G8R8_UNORM,               GL_BGR,             GL_UNSIGNED_BYTE,                  GL_RGB8,               3, GL_FALSE },
    { VK_FORMAT_B8G8R8_SNORM,               GL_BGR,             GL_BYTE,                           GL_RGB8_SNORM,         3, GL_FALSE },
    { VK_FORMAT_B8G8R8_USCALED,             GL_BGR,             GL_UNSIGNED_BYTE,                  GL_RGB8,               3, GL_TRUE  },
    { VK_FORMAT_B8G8R8_SSCALED,             GL_BGR,             GL_BYTE,                           GL_RGB8_SNORM,         3, GL_TRUE  },
    { VK_FORMAT_B8G8R8_UINT,                GL_BGR_INTEGER,     GL_UNSIGNED_BYTE,                  GL_RGB8UI,             3, GL_FALSE },
    { VK_FORMAT_B8G8R8_SINT,                GL_BGR_INTEGER,     GL_BYTE,                           GL_RGB8I,              3, GL_FALSE },

    { VK_FORMAT_R8G8B8A8_UNORM,             GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RGBA8,              4, GL_FALSE },
    { VK_FORMAT_R8G8B8A8_SNORM,             GL_RGBA,            GL_BYTE,                           GL_RGBA8_SNORM,        4, GL_FALSE },
    { VK_FORMAT_R8G8B8A8_USCALED,           GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RGBA8,              4, GL_TRUE  },
    { VK_FORMAT_R8G8B8A8_SSCALED,           GL_RGBA,            GL_BYTE,                           GL_RGBA8_SNORM,        4, GL_TRUE  },
    { VK_FORMAT_R8G8B8A8_UINT,              GL_RGBA_INTEGER,    GL_UNSIGNED_BYTE,                  GL_RGBA8UI,            4, GL_FALSE },
    { VK_FORMAT_R8G8B8A8_SINT,              GL_RGBA_INTEGER,    GL_BYTE,                           GL_RGBA8I,             4, GL_FALSE },
    { VK_FORMAT_R8G8B8A8_SRGB,              GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_SRGB8_ALPHA8,       4, GL_FALSE },

    { VK_FORMAT_B8G8R8A8_UNORM,             GL_BGRA,            GL_UNSIGNED_BYTE,                  GL_RGBA8,              4, GL_FALSE },
    { VK_FORMAT_B8G8R8A8_SNORM,             GL_BGRA,            GL_BYTE,                           GL_RGBA8_SNORM,        4, GL_FALSE },
    { VK_FORMAT_B8G8R8A8_USCALED,           GL_BGRA,            GL_UNSIGNED_BYTE,                  GL_RGBA8,              4, GL_TRUE  },
    { VK_FORMAT_B8G8R8A8_SSCALED,           GL_BGRA,            GL_BYTE,                           GL_RGBA8_SNORM,        4, GL_TRUE  },
    { VK_FORMAT_B8G8R8A8_UINT,              GL_BGRA_INTEGER,    GL_UNSIGNED_BYTE,                  GL_RGBA8UI,            4, GL_FALSE },
    { VK_FORMAT_B8G8R8A8_SINT,              GL_BGRA_INTEGER,    GL_BYTE,                           GL_RGBA8I,             4, GL_FALSE },
    { VK_FORMAT_B8G8R8A8_SRGB,              GL_BGRA,            GL_UNSIGNED_BYTE,                  GL_SRGB8_ALPHA8,       4, GL_FALSE },

    { VK_FORMAT_A8B8G8R8_UNORM_PACK32,      GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8,              4, GL_FALSE },
    { VK_FORMAT_A8B8G8R8_SNORM_PACK32,      GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8_SNORM,        4, GL_FALSE },
    { VK_FORMAT_A8B8G8R8_USCALED_PACK32,    GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8,              4, GL_TRUE  },
    { VK_FORMAT_A8B8G8R8_SSCALED_PACK32,    GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8_SNORM,        4, GL_TRUE  },
    { VK_FORMAT_A8B8G8R8_UINT_PACK32,       GL_RGBA_INTEGER,    GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8UI,            4, GL_FALSE },
    { VK_FORMAT_A8B8G8R8_SINT_PACK32,       GL_RGBA_INTEGER,    GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8I,             4, GL_FALSE },
    { VK_FORMAT_A8B8G8R8_SRGB_PACK32 ,      GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_SRGB8_ALPHA8,       4, GL_FALSE },

    { VK_FORMAT_A2B10G10R10_UNORM_PACK32,   GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2UI,         4, GL_FALSE },
    { VK_FORMAT_A2B10G10R10_SNORM_PACK32,   GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2,           4, GL_FALSE },
    { VK_FORMAT_A2B10G10R10_USCALED_PACK32, GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2UI,         4, GL_TRUE  },
    { VK_FORMAT_A2B10G10R10_SSCALED_PACK32, GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2,           4, GL_TRUE  },
    { VK_FORMAT_A2B10G10R10_UINT_PACK32,    GL_RGBA_INTEGER,    GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2UI,         4, GL_FALSE },
    { VK_FORMAT_A2B10G10R10_SINT_PACK32 ,   GL_RGBA_INTEGER,    GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2,           4, GL_FALSE },

    { VK_FORMAT_R16_UNORM,                  GL_RED,             GL_UNSIGNED_SHORT,                 GL_R16,                1, GL_FALSE },
    { VK_FORMAT_R16_SNORM,                  GL_RED,             GL_SHORT,                          GL_R16_SNORM,          1, GL_FALSE },
    { VK_FORMAT_R16_USCALED,                GL_RED,             GL_UNSIGNED_SHORT,                 GL_R16,                1, GL_TRUE  },
    { VK_FORMAT_R16_SSCALED,                GL_RED,             GL_SHORT,                          GL_R16_SNORM,          1, GL_TRUE  },
    { VK_FORMAT_R16_UINT,                   GL_RED_INTEGER,     GL_UNSIGNED_SHORT,                 GL_R16UI,              1, GL_FALSE },
    { VK_FORMAT_R16_SINT,                   GL_RED_INTEGER,     GL_SHORT,                          GL_R16I,               1, GL_FALSE },
    { VK_FORMAT_R16_SFLOAT,                 GL_RED,             GL_HALF_FLOAT,                     GL_R16F,               1, GL_FALSE },

    { VK_FORMAT_R16G16_UNORM,               GL_RG,              GL_UNSIGNED_SHORT,                 GL_RG16,               2, GL_FALSE },
    { VK_FORMAT_R16G16_SNORM,               GL_RG,              GL_SHORT,                          GL_RG16_SNORM,         2, GL_FALSE },
    { VK_FORMAT_R16G16_USCALED,             GL_RG,              GL_UNSIGNED_SHORT,                 GL_RG16,               2, GL_TRUE  },
    { VK_FORMAT_R16G16_SSCALED,             GL_RG,              GL_SHORT,                          GL_RG16_SNORM,         2, GL_TRUE  },
    { VK_FORMAT_R16G16_UINT,                GL_RG_INTEGER,      GL_UNSIGNED_SHORT,                 GL_RG16UI,             2, GL_FALSE },
    { VK_FORMAT_R16G16_SINT,                GL_RG_INTEGER,      GL_SHORT,                          GL_RG16I,              2, GL_FALSE },
    { VK_FORMAT_R16G16_SFLOAT,              GL_RG,              GL_HALF_FLOAT,                     GL_RG16F,              2, GL_FALSE },

    { VK_FORMAT_R16G16B16_UNORM,            GL_RGB,             GL_UNSIGNED_SHORT,                 GL_RGB16,              3, GL_FALSE },
    { VK_FORMAT_R16G16B16_SNORM,            GL_RGB,             GL_SHORT,                          GL_RGB16_SNORM,        3, GL_FALSE },
    { VK_FORMAT_R16G16B16_USCALED,          GL_RGB,             GL_UNSIGNED_SHORT,                 GL_RGB16,              3, GL_TRUE  },
    { VK_FORMAT_R16G16B16_SSCALED,          GL_RGB,             GL_SHORT,                          GL_RGB16_SNORM,        3, GL_TRUE  },
    { VK_FORMAT_R16G16B16_UINT,             GL_RGB_INTEGER,     GL_UNSIGNED_SHORT,                 GL_RGB16UI,            3, GL_FALSE },
    { VK_FORMAT_R16G16B16_SINT,             GL_RGB_INTEGER,     GL_SHORT,                          GL_RGB16I,             3, GL_FALSE },
    { VK_FORMAT_R16G16B16_SFLOAT,           GL_RGB,             GL_HALF_FLOAT,                     GL_RGB16F,             3, GL_FALSE },

    { VK_FORMAT_R16G16B16A16_UNORM,         GL_RGBA,            GL_UNSIGNED_SHORT,                 GL_RGBA16,             4, GL_FALSE },
    { VK_FORMAT_R16G16B16A16_SNORM,         GL_RGBA,            GL_SHORT,                          GL_RGBA16_SNORM,       4, GL_FALSE },
    { VK_FORMAT_R16G16B16A16_USCALED,       GL_RGBA,            GL_UNSIGNED_SHORT,                 GL_RGBA16,             4, GL_TRUE  },
    { VK_FORMAT_R16G16B16A16_SSCALED,       GL_RGBA,            GL_SHORT,                          GL_RGBA16_SNORM,       4, GL_TRUE  },
    { VK_FORMAT_R16G16B16A16_UINT,          GL_RGBA_INTEGER,    GL_UNSIGNED_SHORT,                 GL_RGBA16UI,           4, GL_FALSE },
    { VK_FORMAT_R16G16B16A16_SINT,          GL_RGBA_INTEGER,    GL_SHORT,                          GL_RGBA16I,            4, GL_FALSE },
    { VK_FORMAT_R16G16B16A16_SFLOAT,        GL_RGBA,            GL_HALF_FLOAT,                     GL_RGBA16F,            4, GL_FALSE },

    { VK_FORMAT_R32_UINT,                   GL_RED_INTEGER,     GL_UNSIGNED_INT,                   GL_R32UI,              1, GL_FALSE },
    { VK_FORMAT_R32_SINT,                   GL_RED_INTEGER,     GL_INT,                            GL_R32I,               1, GL_FALSE },
    { VK_FORMAT_R32_SFLOAT,                 GL_RED,             GL_FLOAT,                          GL_R32F,               1, GL_FALSE },

    { VK_FORMAT_R32G32_UINT,                GL_RG_INTEGER,      GL_UNSIGNED_INT,                   GL_RG32UI,             2, GL_FALSE },
    { VK_FORMAT_R32G32_SINT,                GL_RG_INTEGER,      GL_INT,                            GL_RG32I,              2, GL_FALSE },
    { VK_FORMAT_R32G32_SFLOAT,              GL_RG,              GL_FLOAT,                          GL_RG32F,              2, GL_FALSE },

    { VK_FORMAT_R32G32B32_UINT,             GL_RGB_INTEGER,     GL_UNSIGNED_INT,                   GL_RGB32UI,            3, GL_FALSE },
    { VK_FORMAT_R32G32B32_SINT,             GL_RGB_INTEGER,     GL_INT,                            GL_RGB32I,             3, GL_FALSE },
    { VK_FORMAT_R32G32B32_SFLOAT,           GL_RGB,             GL_FLOAT,                          GL_RGB32F,             3, GL_FALSE },

    { VK_FORMAT_R32G32B32A32_UINT,          GL_RGBA_INTEGER,    GL_UNSIGNED_INT,                   GL_RGBA32UI,           4, GL_FALSE },
    { VK_FORMAT_R32G32B32A32_SINT,          GL_RGBA_INTEGER,    GL_INT,                            GL_RGBA32I,            4, GL_FALSE },
    { VK_FORMAT_R32G32B32A32_SFLOAT,        GL_RGBA,            GL_FLOAT,                          GL_RGBA32F,            4, GL_FALSE },

    { VK_FORMAT_B10G11R11_UFLOAT_PACK32,    GL_RGB,             GL_UNSIGNED_INT_10F_11F_11F_REV,   GL_R11F_G11F_B10F,     3, GL_FALSE },
    { VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,     GL_RGB,             GL_UNSIGNED_INT_5_9_9_9_REV,       GL_RGB9_E5,            3, GL_FALSE },

    { VK_FORMAT_D16_UNORM,                  GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT,                 GL_DEPTH_COMPONENT16,  1, GL_FALSE },
    { VK_FORMAT_X8_D24_UNORM_PACK32,        GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,                   GL_DEPTH_COMPONENT24,  1, GL_FALSE },
    { VK_FORMAT_D32_SFLOAT,                 GL_DEPTH_COMPONENT, GL_FLOAT,                          GL_DEPTH_COMPONENT32F, 1, GL_FALSE },
    { VK_FORMAT_S8_UINT,                    GL_STENCIL_INDEX,   GL_UNSIGNED_BYTE,                  GL_STENCIL_INDEX8,     1, GL_FALSE },
    { VK_FORMAT_D24_UNORM_S8_UINT,          GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8,              GL_DEPTH24_STENCIL8,   2, GL_FALSE },
    { VK_FORMAT_D32_SFLOAT_S8_UINT,         GL_DEPTH_STENCIL,   GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_DEPTH32F_STENCIL8,  2, GL_FALSE },
};

static const GLenum sBlendFactors[] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
    GL_SRC_ALPHA_SATURATE,
    GL_SRC1_COLOR,
    GL_ONE_MINUS_SRC1_COLOR,
    GL_SRC1_ALPHA,
    GL_ONE_MINUS_SRC1_ALPHA
};

static const GLenum sBlendOperations[] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX
};

static const GLenum sLogicOps[] = {
    GL_CLEAR,         // VK_LOGIC_OP_CLEAR = 0,
    GL_AND,           // VK_LOGIC_OP_AND = 1,
    GL_AND_REVERSE,   // VK_LOGIC_OP_AND_REVERSE = 2,
    GL_COPY,          // VK_LOGIC_OP_COPY = 3,
    GL_AND_INVERTED,  // VK_LOGIC_OP_AND_INVERTED = 4,
    GL_NOOP,          // VK_LOGIC_OP_NO_OP = 5,
    GL_XOR,           // VK_LOGIC_OP_XOR = 6,
    GL_OR,            // VK_LOGIC_OP_OR = 7,
    GL_NOR,           // VK_LOGIC_OP_NOR = 8,
    GL_EQUIV,         // VK_LOGIC_OP_EQUIVALENT = 9,
    GL_INVERT,        // VK_LOGIC_OP_INVERT = 10,
    GL_OR_REVERSE,    // VK_LOGIC_OP_OR_REVERSE = 11,
    GL_COPY_INVERTED, // VK_LOGIC_OP_COPY_INVERTED = 12,
    GL_OR_INVERTED,   // VK_LOGIC_OP_OR_INVERTED = 13,
    GL_NAND,          // VK_LOGIC_OP_NAND = 14,
    GL_SET,           // VK_LOGIC_OP_SET = 15,
};

static const uint32_t sDynamicStates[] = {
    CTS_GL_DYNAMIC_STATE_VIEWPORT_BIT,
    CTS_GL_DYNAMIC_STATE_SCISSOR_BIT,
    CTS_GL_DYNAMIC_STATE_LINE_WIDTH_BIT,
    CTS_GL_DYNAMIC_STATE_DEPTH_BIAS_BIT,
    CTS_GL_DYNAMIC_STATE_BLEND_CONSTANTS_BIT,
    CTS_GL_DYNAMIC_STATE_DEPTH_BOUNDS_BIT,
    CTS_GL_DYNAMIC_STATE_STENCIL_COMPARE_MASK_BIT,
    CTS_GL_DYNAMIC_STATE_STENCIL_WRITE_MASK_BIT,
    CTS_GL_DYNAMIC_STATE_STENCIL_REFERENCE_BIT
};

static const bool sBorderColorIsFloat[] = {
    true,  // VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
    false, // VK_BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
    true,  // VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
    false, // VK_BORDER_COLOR_INT_OPAQUE_BLACK = 3,
    true,  // VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
    false, // VK_BORDER_COLOR_INT_OPAQUE_WHITE = 5,
};

static const float sBorderColorsFloat[][4] = {
    {0.0f, 0.0f, 0.0f, 0.0f}, // VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
    {0.0f, 0.0f, 0.0f, 0.0f}, // VK_BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
    {0.0f, 0.0f, 0.0f, 1.0f}, // VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
    {0.0f, 0.0f, 0.0f, 1.0f}, // VK_BORDER_COLOR_INT_OPAQUE_BLACK = 3,
    {1.0f, 1.0f, 1.0f, 1.0f}, // VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
    {1.0f, 1.0f, 1.0f, 1.0f}, // VK_BORDER_COLOR_INT_OPAQUE_WHITE = 5,
};

static const int sBorderColorsInt[][4] = {
    {0, 0, 0, 0}, // VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
    {0, 0, 0, 0}, // VK_BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
    {0, 0, 0, 1}, // VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
    {0, 0, 0, 1}, // VK_BORDER_COLOR_INT_OPAQUE_BLACK = 3,
    {1, 1, 1, 1}, // VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
    {1, 1, 1, 1}, // VK_BORDER_COLOR_INT_OPAQUE_WHITE = 5,
};
/*
static_assert(COUNTOF(sCompareOps) == NUM_CTS_COMPARE_OPS, "Assertion failure");
static_assert(COUNTOF(sStencilOps) == NUM_CTS_STENCIL_OPS, "Assertion failure");
static_assert(COUNTOF(sFrontFaces) == NUM_CTS_FRONT_FACES, "Assertion failure");
static_assert(COUNTOF(sPrimitiveTopologies) == NUM_CTS_PRIMITIVE_TOPOLOGIES, "Assertion failure");
static_assert(COUNTOF(sPolygonModes) == NUM_CTS_POLYGON_MODES, "Assertion failure");
static_assert(COUNTOF(sSamplerAddressModes) == NUM_CTS_SAMPLER_ADDRESS_MODES, "Assertion failure");
static_assert(COUNTOF(sMagFilters) == NUM_CTS_FILTERS, "Assertion failure");
static_assert(COUNTOF(sMinFiltersNearest) == NUM_CTS_FILTERS, "Assertion failure");
static_assert(COUNTOF(sMinFiltersLinear) == NUM_CTS_FILTERS, "Assertion failure");
static_assert(COUNTOF(sImageTypes) == NUM_CTS_IMAGE_TYPES, "Assertion failure");
static_assert(COUNTOF(sImageArrayTypes) == NUM_CTS_IMAGE_TYPES, "Assertion failure");
static_assert(COUNTOF(sMultisampleImageTypes) == NUM_CTS_IMAGE_TYPES, "Assertion failure");
static_assert(COUNTOF(sMultisampleImageArrayTypes) == NUM_CTS_IMAGE_TYPES, "Assertion failure");
static_assert(COUNTOF(sImageViewTypes) == NUM_CTS_IMAGE_VIEW_TYPES, "Assertion failure");
static_assert(COUNTOF(sMultisampleImageViewTypes) == NUM_CTS_IMAGE_VIEW_TYPES, "Assertion failure");
static_assert(COUNTOF(sFormats) == NUM_CTS_FORMATS, "Assertion failure");
static_assert(COUNTOF(sBlendFactors) == NUM_CTS_BLEND_FACTORS, "Assertion failure");
static_assert(COUNTOF(sBlendOperations) == NUM_CTS_BLEND_OPS, "Assertion failure");
static_assert(COUNTOF(sDynamicStates) == NUM_CTS_DYNAMIC_STATES, "Assertion failure");
static_assert(COUNTOF(sBorderColorIsFloat) == NUM_CTS_BORDER_COLORS, "Assertion failure");
static_assert(COUNTOF(sBorderColorsFloat) == NUM_CTS_BORDER_COLORS, "Assertion failure");
static_assert(COUNTOF(sBorderColorsInt) == NUM_CTS_BORDER_COLORS, "Assertion failure");
*/
const GLenum parseCompareOp(VkCompareOp value)
{
    return sCompareOps[value];
}

const GLenum parseStencilOp(VkStencilOp value)
{
    return sStencilOps[value];
}

const GLuint parseCullMode(VkCullModeFlags value)
{
    GLuint result = 0;

    if (value & VK_CULL_MODE_FRONT_BIT) {
        result |= GL_FRONT;
    }

    if (value & VK_CULL_MODE_BACK_BIT) {
        result |= GL_BACK;
    }

    return result;
}

const GLenum parseFrontFace(VkFrontFace value)
{
    return sFrontFaces[value];
}

const GLenum parsePrimitiveTopology(VkPrimitiveTopology value)
{
    return sPrimitiveTopologies[value];
}

const GLenum parsePolygonMode(VkPolygonMode value)
{
    return sPolygonModes[value];
}

const GLenum parseSamplerAddressMode(VkSamplerAddressMode value)
{
    return sSamplerAddressModes[value];
}

const GLenum parseMinFilter(VkFilter filter, VkSamplerMipmapMode mipmapMode)
{
    return (mipmapMode == VK_SAMPLER_MIPMAP_MODE_LINEAR)
        ? sMinFiltersLinear[filter]
        : sMinFiltersNearest[filter];
}

const GLenum parseMagFilter(VkFilter filter, VkSamplerMipmapMode mipmapMode)
{
    return sMagFilters[filter];
}

const GLenum parseShaderType(VkShaderStageFlags value)
{
    if (value & VK_SHADER_STAGE_VERTEX_BIT) {
        return GL_VERTEX_SHADER;
    }

    if (value & VK_SHADER_STAGE_FRAGMENT_BIT) {
        return GL_FRAGMENT_SHADER;
    }

    if (value & VK_SHADER_STAGE_GEOMETRY_BIT) {
        return GL_GEOMETRY_SHADER;
    }

    if (value & VK_SHADER_STAGE_COMPUTE_BIT) {
        return GL_COMPUTE_SHADER;
    }

    //CTS_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
    //CTS_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
    //CTS_SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
    //CTS_SHADER_STAGE_ALL = 0x7FFFFFFF
    
    return GL_INVALID_ENUM;
}

const GLenum parseImageType(VkImageType value, bool isMultisample, bool isArray)
{
    return isMultisample
        ? isArray
            ? sMultisampleImageArrayTypes[value]
            : sMultisampleImageTypes[value]
        : isArray
            ? sImageArrayTypes[value]
            : sImageTypes[value];
}

const GLenum parseImageViewType(VkImageViewType value, bool isMultisample)
{
    return isMultisample
        ? sMultisampleImageViewTypes[value]
        : sImageViewTypes[value];
}

const GLenum parseBufferUsage(VkBufferUsageFlags value)
{
    if (value & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ||
        value & VK_BUFFER_USAGE_TRANSFER_SRC_BIT ||
        value & VK_BUFFER_USAGE_TRANSFER_DST_BIT ||
        value & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
    ) {
        return GL_ARRAY_BUFFER;
    }

    if (value & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }

    if (value & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        return GL_UNIFORM_BUFFER;
    }

    if (value & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) {
        return GL_TEXTURE_BUFFER;
    }

    if (value & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        return GL_INVALID_ENUM; // GL_SHADER_STORAGE_BUFFER;
    }

    if (value & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)  {
        return GL_INVALID_ENUM; // GL_SHADER_STORAGE_BUFFER;
    }
 
    return GL_INVALID_ENUM;
}

const GLenum parseBlendFactor(VkBlendFactor value)
{
    return sBlendFactors[value];
}

const GLenum parseBlendOperation(VkBlendOp value)
{
    return sBlendOperations[value];
}

const GLenum parseLogicOp(VkLogicOp value)
{
    return sLogicOps[value];
}

const VkFlags parseDynamicStateFlag(VkDynamicState value)
{
    return sDynamicStates[value];
}

const CtsFormatData* parseFormat(VkFormat value)
{
    for (int i = 0; i < COUNTOF(sFormats); ++i) {
        if (sFormats[i].vkFormat == value) {
            return &sFormats[i];
        }
    }

    return NULL;
}

const bool isFloatBorderColor(VkBorderColor borderColor)
{
    return sBorderColorIsFloat[borderColor];
}

const float* parseBorderColorFloat(VkBorderColor borderColor)
{
    return sBorderColorsFloat[borderColor];
}

const int* parseBorderColorInt(VkBorderColor borderColor)
{
    return sBorderColorsInt[borderColor];
}

void parseRasterizationStateChanges(
    const CtsGlPipelineRasterizationState* pLhs, 
    const CtsGlPipelineRasterizationState* pRhs, 
    CtsGlRasterizationStateChanges* pChanges
) {
    if (pLhs == NULL || pRhs == NULL) {
        memset(pChanges, true, sizeof(CtsGlRasterizationStateChanges));
        return;
    }

    pChanges->polygonModeChanged             = pLhs->polygonMode             != pRhs->polygonMode;
    pChanges->cullModeChanged                = pLhs->cullMode                != pRhs->cullMode;
    pChanges->frontFaceChanged               = pLhs->frontFace               != pRhs->frontFace;

    pChanges->depthBiasChanged = 
        (pLhs->depthBiasEnable != pRhs->depthBiasEnable)                                  ||
        (fabsf(pLhs->depthBiasConstantFactor - pRhs->depthBiasConstantFactor) >= EPSILON) ||
        (fabsf(pLhs->depthBiasClamp          - pRhs->depthBiasClamp)          >= EPSILON) ||
        (fabsf(pLhs->depthBiasSlopeFactor    - pRhs->depthBiasSlopeFactor)    >= EPSILON) ||
        (fabsf(pLhs->lineWidth               - pRhs->lineWidth)               >= EPSILON);
}

void parseDepthStencilStateChanges(
    const CtsGlPipelineDepthStencilState* pLhs, 
    const CtsGlPipelineDepthStencilState* pRhs, 
    CtsGlDepthStencilStateChanges* pChanges
) {
    if (pLhs == NULL || pRhs == NULL) {
        memset(pChanges, true, sizeof(CtsGlDepthStencilStateChanges));
        return;
    }

    pChanges->depthWriteEnableChanged      = pLhs->depthWriteEnable      != pRhs->depthWriteEnable;
    pChanges->depthCompareOpChanged        = pLhs->depthCompareOp        != pRhs->depthCompareOp;
    pChanges->depthBoundsTestEnableChanged = pLhs->depthBoundsTestEnable != pRhs->depthBoundsTestEnable;

    pChanges->depthBoundsChanged = 
        (fabsf(pLhs->minDepthBounds - pRhs->minDepthBounds) >= EPSILON) ||
        (fabsf(pLhs->maxDepthBounds - pRhs->maxDepthBounds) >= EPSILON);

    pChanges->frontFuncChanged = 
        (pLhs->frontCompareOp   != pRhs->frontCompareOp)   ||
        (pLhs->frontCompareMask != pRhs->frontCompareMask) ||
        (pLhs->frontReference   != pRhs->frontReference);

    pChanges->backFuncChanged = 
        (pLhs->backCompareOp    != pRhs->backCompareOp)    ||
        (pLhs->backCompareMask  != pRhs->backCompareMask)  ||
        (pLhs->backReference    != pRhs->backReference);

    pChanges->frontStencilChanged = 
        (pLhs->frontFailOp      != pRhs->frontFailOp)      ||
        (pLhs->frontDepthFailOp != pRhs->frontDepthFailOp) ||
        (pLhs->frontPassOp      != pRhs->frontPassOp);

    pChanges->backStencilChanged =
        (pLhs->backFailOp       != pRhs->backFailOp)       ||
        (pLhs->backDepthFailOp  != pRhs->backDepthFailOp)  ||
        (pLhs->backPassOp       != pRhs->backPassOp);

    pChanges->frontFaceWriteMaskChanged = pLhs->frontWriteMask != pRhs->frontWriteMask;
    pChanges->backFaceWriteMaskChanged  = pLhs->backWriteMask  != pRhs->backWriteMask;
}

bool blendStateBlendConstantChanged(
    const CtsGlPipelineColorBlendState* pLhs,
    const CtsGlPipelineColorBlendState* pRhs
) {
    if (pLhs == NULL || pRhs == NULL) {
        return true;
    }

    return (
        (fabsf(pLhs->blendConstants[0] - pRhs->blendConstants[0]) >= EPSILON) ||
        (fabsf(pLhs->blendConstants[1] - pRhs->blendConstants[1]) >= EPSILON) ||
        (fabsf(pLhs->blendConstants[2] - pRhs->blendConstants[2]) >= EPSILON) ||
        (fabsf(pLhs->blendConstants[3] - pRhs->blendConstants[3]) >= EPSILON)
    );
}

void parseColorBlendStateChanges(
    const CtsGlPipelineColorBlendStateAttachment* pLhs, 
    const CtsGlPipelineColorBlendStateAttachment* pRhs, 
    CtsGlColorBlendStateChanges* pChanges
) {
    if (pLhs == NULL || pRhs == NULL) {
        memset(pChanges, true, sizeof(CtsGlColorBlendStateChanges));
        return;
    }

    pChanges->colorWriteMaskChanged = pLhs->colorWriteMask != pRhs->colorWriteMask;

    pChanges->blendFactorChanged = 
        pLhs->srcColorBlendFactor != pRhs->srcColorBlendFactor ||
        pLhs->dstColorBlendFactor != pRhs->dstColorBlendFactor ||
        pLhs->srcAlphaBlendFactor != pRhs->srcAlphaBlendFactor ||
        pLhs->dstAlphaBlendFactor != pRhs->dstAlphaBlendFactor;

    pChanges->blendOpChanged = 
        pLhs->colorBlendOp != pRhs->colorBlendOp ||
        pLhs->alphaBlendOp != pRhs->alphaBlendOp;


}

uint32_t parseDynamicStateFlagBits(const VkPipelineDynamicStateCreateInfo* pCreateInfo) {
    if (pCreateInfo == NULL) {
        return 0;
    }

    uint32_t dynamicState = 0;
    for (uint32_t i = 0; i < pCreateInfo->dynamicStateCount; ++i) {
        dynamicState |= parseDynamicStateFlag(pCreateInfo->pDynamicStates[i]);
    }

    return dynamicState;
}

#ifdef __cplusplus
}
#endif