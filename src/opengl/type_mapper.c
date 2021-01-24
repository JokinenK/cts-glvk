#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <cts/typedefs/gl_enums.h>
#include <cts/type_mapper.h>

#ifndef COUNTOF
#  define COUNTOF(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

static const float EPSILON = 1.0f / 1000.0f;

static const GLenum sCompareOps[] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

static const GLenum sStencilOps[] = {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_INCR_WRAP,
    GL_DECR,
    GL_DECR_WRAP,
    GL_INVERT
};

static const GLenum sFrontFaces[] = {
    GL_CW,
    GL_CCW
};

static const GLenum sPrimitiveTopologies[] = {
    GL_POINTS,
    GL_LINES,
    GL_LINE_STRIP,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN,
    GL_LINES_ADJACENCY,
    GL_LINE_STRIP_ADJACENCY,
    GL_TRIANGLES_ADJACENCY,
    GL_TRIANGLE_STRIP_ADJACENCY,
    GL_INVALID_ENUM, //GL_PATCHES
};

static const GLenum sPolygonModes[] = {
    GL_POINT,
    GL_LINE,
    GL_FILL
};

static const GLenum sSamplerAddressModes[] = {
    GL_REPEAT,
    GL_MIRRORED_REPEAT,
    GL_CLAMP_TO_EDGE,
    GL_CLAMP_TO_BORDER
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

static GLenum sImageViewTypes[] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_CUBE_MAP_ARRAY_ARB
};

static const CtsFormatData sFormats[] = {
    { GL_INVALID_ENUM,    GL_INVALID_ENUM,                   GL_INVALID_ENUM,       0, GL_FALSE }, // CTS_FORMAT_UNDEFINED

    { GL_RGBA,            GL_UNSIGNED_SHORT_4_4_4_4,         GL_RGBA4,              4, GL_FALSE }, // CTS_FORMAT_R4G4B4A4_UNORM_PACK16
    { GL_BGRA,            GL_UNSIGNED_SHORT_4_4_4_4_REV,     GL_RGBA4,              4, GL_FALSE }, // CTS_FORMAT_B4G4R4A4_UNORM_PACK16

    { GL_RGB,             GL_UNSIGNED_SHORT_5_6_5,           GL_RGB8,               3, GL_FALSE }, // CTS_FORMAT_R5G6B5_UNORM_PACK16
    { GL_RGB,             GL_UNSIGNED_SHORT_5_6_5,           GL_RGB8,               3, GL_FALSE }, // CTS_FORMAT_B5G6R5_UNORM_PACK16

    { GL_RGBA,            GL_UNSIGNED_SHORT_5_5_5_1,         GL_RGB5_A1,            4, GL_FALSE }, // CTS_FORMAT_R5G5B5A1_UNORM_PACK16
    { GL_BGRA,            GL_UNSIGNED_SHORT_5_5_5_1,         GL_RGB5_A1,            4, GL_FALSE }, // CTS_FORMAT_B5G5R5A1_UNORM_PACK16
    { GL_RGBA,            GL_UNSIGNED_SHORT_1_5_5_5_REV,     GL_RGB5_A1,            4, GL_FALSE }, // CTS_FORMAT_A1R5G5B5_UNORM_PACK16

    { GL_RED,             GL_UNSIGNED_BYTE,                  GL_R8,                 1, GL_FALSE }, // CTS_FORMAT_R8_UNORM
    { GL_RED,             GL_BYTE,                           GL_R8_SNORM,           1, GL_FALSE }, // CTS_FORMAT_R8_SNORM 
    { GL_RED,             GL_UNSIGNED_BYTE,                  GL_R8,                 1, GL_TRUE  }, // CTS_FORMAT_R8_USCALED
    { GL_RED,             GL_BYTE,                           GL_R8_SNORM,           1, GL_TRUE  }, // CTS_FORMAT_R8_SSCALED 
    { GL_RED_INTEGER,     GL_UNSIGNED_BYTE,                  GL_R8UI,               1, GL_FALSE }, // CTS_FORMAT_R8_UINT 
    { GL_RED_INTEGER,     GL_BYTE,                           GL_R8I,                1, GL_FALSE }, // CTS_FORMAT_R8_SINT 

    { GL_RG,              GL_UNSIGNED_BYTE,                  GL_RG8,                2, GL_FALSE }, // CTS_FORMAT_R8G8_UNORM 
    { GL_RG,              GL_BYTE,                           GL_RG8_SNORM,          2, GL_FALSE }, // CTS_FORMAT_R8G8_SNORM 
    { GL_RG,              GL_UNSIGNED_BYTE,                  GL_RG8,                2, GL_TRUE  }, // CTS_FORMAT_R8G8_USCALED
    { GL_RG,              GL_BYTE,                           GL_RG8_SNORM,          2, GL_TRUE  }, // CTS_FORMAT_R8G8_SSCALED
    { GL_RG_INTEGER,      GL_UNSIGNED_BYTE,                  GL_RG8UI,              2, GL_FALSE }, // CTS_FORMAT_R8G8_UINT 
    { GL_RG_INTEGER,      GL_BYTE,                           GL_RG8I,               2, GL_FALSE }, // CTS_FORMAT_R8G8_SINT 

    { GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RGB8,               3, GL_FALSE }, // CTS_FORMAT_R8G8B8_UNORM 
    { GL_RGB,             GL_BYTE,                           GL_RGB8_SNORM,         3, GL_FALSE }, // CTS_FORMAT_R8G8B8_SNORM 
    { GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RGB8,               3, GL_TRUE  }, // CTS_FORMAT_R8G8B8_USCALED
    { GL_RGB,             GL_BYTE,                           GL_RGB8_SNORM,         3, GL_TRUE  }, // CTS_FORMAT_R8G8B8_SSCALED 
    { GL_RGB_INTEGER,     GL_UNSIGNED_BYTE,                  GL_RGB8UI,             3, GL_FALSE }, // CTS_FORMAT_R8G8B8_UINT 
    { GL_RGB_INTEGER,     GL_BYTE,                           GL_RGB8I,              3, GL_FALSE }, // CTS_FORMAT_R8G8B8_SINT 
    { GL_RGB,             GL_UNSIGNED_BYTE,                  GL_SRGB8,              3, GL_FALSE }, // CTS_FORMAT_R8G8B8_SRGB 

    { GL_BGR,             GL_UNSIGNED_BYTE,                  GL_RGB8,               3, GL_FALSE }, // CTS_FORMAT_B8G8R8_UNORM 
    { GL_BGR,             GL_BYTE,                           GL_RGB8_SNORM,         3, GL_FALSE }, // CTS_FORMAT_B8G8R8_SNORM 
    { GL_BGR,             GL_UNSIGNED_BYTE,                  GL_RGB8,               3, GL_TRUE  }, // CTS_FORMAT_B8G8R8_USCALED
    { GL_BGR,             GL_BYTE,                           GL_RGB8_SNORM,         3, GL_TRUE  }, // CTS_FORMAT_B8G8R8_SSCALED
    { GL_BGR_INTEGER,     GL_UNSIGNED_BYTE,                  GL_RGB8UI,             3, GL_FALSE }, // CTS_FORMAT_B8G8R8_UINT 
    { GL_BGR_INTEGER,     GL_BYTE,                           GL_RGB8I,              3, GL_FALSE }, // CTS_FORMAT_B8G8R8_SINT 

    { GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RGBA8,              4, GL_FALSE }, // CTS_FORMAT_R8G8B8A8_UNORM 
    { GL_RGBA,            GL_BYTE,                           GL_RGBA8_SNORM,        4, GL_FALSE }, // CTS_FORMAT_R8G8B8A8_SNORM 
    { GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RGBA8,              4, GL_TRUE  }, // CTS_FORMAT_R8G8B8A8_USCALED
    { GL_RGBA,            GL_BYTE,                           GL_RGBA8_SNORM,        4, GL_TRUE  }, // CTS_FORMAT_R8G8B8A8_SSCALED
    { GL_RGBA_INTEGER,    GL_UNSIGNED_BYTE,                  GL_RGBA8UI,            4, GL_FALSE }, // CTS_FORMAT_R8G8B8A8_UINT 
    { GL_RGBA_INTEGER,    GL_BYTE,                           GL_RGBA8I,             4, GL_FALSE }, // CTS_FORMAT_R8G8B8A8_SINT 
    { GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_SRGB8_ALPHA8,       4, GL_FALSE }, // CTS_FORMAT_R8G8B8A8_SRGB 

    { GL_BGRA,            GL_UNSIGNED_BYTE,                  GL_RGBA8,              4, GL_FALSE }, // CTS_FORMAT_B8G8R8A8_UNORM 
    { GL_BGRA,            GL_BYTE,                           GL_RGBA8_SNORM,        4, GL_FALSE }, // CTS_FORMAT_B8G8R8A8_SNORM 
    { GL_BGRA,            GL_UNSIGNED_BYTE,                  GL_RGBA8,              4, GL_TRUE  }, // CTS_FORMAT_B8G8R8A8_USCALED
    { GL_BGRA,            GL_BYTE,                           GL_RGBA8_SNORM,        4, GL_TRUE  }, // CTS_FORMAT_B8G8R8A8_SSCALED
    { GL_BGRA_INTEGER,    GL_UNSIGNED_BYTE,                  GL_RGBA8UI,            4, GL_FALSE }, // CTS_FORMAT_B8G8R8A8_UINT 
    { GL_BGRA_INTEGER,    GL_BYTE,                           GL_RGBA8I,             4, GL_FALSE }, // CTS_FORMAT_B8G8R8A8_SINT 
    { GL_BGRA,            GL_UNSIGNED_BYTE,                  GL_SRGB8_ALPHA8,       4, GL_FALSE }, // CTS_FORMAT_B8G8R8A8_SRGB 

    { GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8,              4, GL_FALSE }, // CTS_FORMAT_A8B8G8R8_UNORM_PACK32
    { GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8_SNORM,        4, GL_FALSE }, // CTS_FORMAT_A8B8G8R8_SNORM_PACK32
    { GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8,              4, GL_TRUE  }, // CTS_FORMAT_A8B8G8R8_USCALED_PACK32
    { GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8_SNORM,        4, GL_TRUE  }, // CTS_FORMAT_A8B8G8R8_SSCALED_PACK32
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8UI,            4, GL_FALSE }, // CTS_FORMAT_A8B8G8R8_UINT_PACK32
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8I,             4, GL_FALSE }, // CTS_FORMAT_A8B8G8R8_SINT_PACK32
    { GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_SRGB8_ALPHA8,       4, GL_FALSE }, // CTS_FORMAT_A8B8G8R8_SRGB_PACK32 

    { GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2UI,         4, GL_FALSE }, // CTS_FORMAT_A2B10G10R10_UNORM_PACK32
    { GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2,           4, GL_FALSE }, // CTS_FORMAT_A2B10G10R10_SNORM_PACK32
    { GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2UI,         4, GL_TRUE  }, // CTS_FORMAT_A2B10G10R10_USCALED_PACK32
    { GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2,           4, GL_TRUE  }, // CTS_FORMAT_A2B10G10R10_SSCALED_PACK32
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2UI,         4, GL_FALSE }, // CTS_FORMAT_A2B10G10R10_UINT_PACK32
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2,           4, GL_FALSE }, // CTS_FORMAT_A2B10G10R10_SINT_PACK32 

    { GL_RED,             GL_UNSIGNED_SHORT,                 GL_R16,                1, GL_FALSE }, // CTS_FORMAT_R16_UNORM 
    { GL_RED,             GL_SHORT,                          GL_R16_SNORM,          1, GL_FALSE }, // CTS_FORMAT_R16_SNORM 
    { GL_RED,             GL_UNSIGNED_SHORT,                 GL_R16,                1, GL_TRUE  }, // CTS_FORMAT_R16_USCALED
    { GL_RED,             GL_SHORT,                          GL_R16_SNORM,          1, GL_TRUE  }, // CTS_FORMAT_R16_SSCALED
    { GL_RED_INTEGER,     GL_UNSIGNED_SHORT,                 GL_R16UI,              1, GL_FALSE }, // CTS_FORMAT_R16_UINT 
    { GL_RED_INTEGER,     GL_SHORT,                          GL_R16I,               1, GL_FALSE }, // CTS_FORMAT_R16_SINT 
    { GL_RED,             GL_HALF_FLOAT,                     GL_R16F,               1, GL_FALSE }, // CTS_FORMAT_R16_SFLOAT 

    { GL_RG,              GL_UNSIGNED_SHORT,                 GL_RG16,               2, GL_FALSE }, // CTS_FORMAT_R16G16_UNORM 
    { GL_RG,              GL_SHORT,                          GL_RG16_SNORM,         2, GL_FALSE }, // CTS_FORMAT_R16G16_SNORM 
    { GL_RG,              GL_UNSIGNED_SHORT,                 GL_RG16,               2, GL_TRUE  }, // CTS_FORMAT_R16G16_USCALED
    { GL_RG,              GL_SHORT,                          GL_RG16_SNORM,         2, GL_TRUE  }, // CTS_FORMAT_R16G16_SSCALED
    { GL_RG_INTEGER,      GL_UNSIGNED_SHORT,                 GL_RG16UI,             2, GL_FALSE }, // CTS_FORMAT_R16G16_UINT 
    { GL_RG_INTEGER,      GL_SHORT,                          GL_RG16I,              2, GL_FALSE }, // CTS_FORMAT_R16G16_SINT 
    { GL_RG,              GL_HALF_FLOAT,                     GL_RG16F,              2, GL_FALSE }, // CTS_FORMAT_R16G16_SFLOAT 

    { GL_RGB,             GL_UNSIGNED_SHORT,                 GL_RGB16,              3, GL_FALSE }, // CTS_FORMAT_R16G16B16_UNORM 
    { GL_RGB,             GL_SHORT,                          GL_RGB16_SNORM,        3, GL_FALSE }, // CTS_FORMAT_R16G16B16_SNORM 
    { GL_RGB,             GL_UNSIGNED_SHORT,                 GL_RGB16,              3, GL_TRUE  }, // CTS_FORMAT_R16G16B16_USCALED
    { GL_RGB,             GL_SHORT,                          GL_RGB16_SNORM,        3, GL_TRUE  }, // CTS_FORMAT_R16G16B16_SSCALED
    { GL_RGB_INTEGER,     GL_UNSIGNED_SHORT,                 GL_RGB16UI,            3, GL_FALSE }, // CTS_FORMAT_R16G16B16_UINT 
    { GL_RGB_INTEGER,     GL_SHORT,                          GL_RGB16I,             3, GL_FALSE }, // CTS_FORMAT_R16G16B16_SINT 
    { GL_RGB,             GL_HALF_FLOAT,                     GL_RGB16F,             3, GL_FALSE }, // CTS_FORMAT_R16G16B16_SFLOAT 

    { GL_RGBA,            GL_UNSIGNED_SHORT,                 GL_RGBA16,             4, GL_FALSE }, // CTS_FORMAT_R16G16B16A16_UNORM 
    { GL_RGBA,            GL_SHORT,                          GL_RGBA16_SNORM,       4, GL_FALSE }, // CTS_FORMAT_R16G16B16A16_SNORM 
    { GL_RGBA,            GL_UNSIGNED_SHORT,                 GL_RGBA16,             4, GL_TRUE  }, // CTS_FORMAT_R16G16B16A16_USCALED
    { GL_RGBA,            GL_SHORT,                          GL_RGBA16_SNORM,       4, GL_TRUE  }, // CTS_FORMAT_R16G16B16A16_SSCALED
    { GL_RGBA_INTEGER,    GL_UNSIGNED_SHORT,                 GL_RGBA16UI,           4, GL_FALSE }, // CTS_FORMAT_R16G16B16A16_UINT 
    { GL_RGBA_INTEGER,    GL_SHORT,                          GL_RGBA16I,            4, GL_FALSE }, // CTS_FORMAT_R16G16B16A16_SINT 
    { GL_RGBA,            GL_HALF_FLOAT,                     GL_RGBA16F,            4, GL_FALSE }, // CTS_FORMAT_R16G16B16A16_SFLOAT 

    { GL_RED_INTEGER,     GL_UNSIGNED_INT,                   GL_R32UI,              1, GL_FALSE }, // CTS_FORMAT_R32_UINT 
    { GL_RED_INTEGER,     GL_INT,                            GL_R32I,               1, GL_FALSE }, // CTS_FORMAT_R32_SINT 
    { GL_RED_INTEGER,     GL_UNSIGNED_INT,                   GL_R32UI,              1, GL_TRUE  }, // CTS_FORMAT_R32_USCALED
    { GL_RED_INTEGER,     GL_INT,                            GL_R32I,               1, GL_TRUE  }, // CTS_FORMAT_R32_SSCALED
    { GL_RED,             GL_FLOAT,                          GL_R32F,               1, GL_FALSE }, // CTS_FORMAT_R32_SFLOAT

    { GL_RG_INTEGER,      GL_UNSIGNED_INT,                   GL_RG32UI,             2, GL_FALSE }, // CTS_FORMAT_R32G32_UINT
    { GL_RG_INTEGER,      GL_INT,                            GL_RG32I,              2, GL_FALSE }, // CTS_FORMAT_R32G32_SINT
    { GL_RG_INTEGER,      GL_UNSIGNED_INT,                   GL_RG32UI,             2, GL_TRUE  }, // CTS_FORMAT_R32G32_USCALED
    { GL_RG_INTEGER,      GL_INT,                            GL_RG32I,              2, GL_TRUE  }, // CTS_FORMAT_R32G32_SSCALED
    { GL_RG,              GL_FLOAT,                          GL_RG32F,              2, GL_FALSE }, // CTS_FORMAT_R32G32_SFLOAT

    { GL_RGB_INTEGER,     GL_UNSIGNED_INT,                   GL_RGB32UI,            3, GL_FALSE }, // CTS_FORMAT_R32G32B32_UINT
    { GL_RGB_INTEGER,     GL_INT,                            GL_RGB32I,             3, GL_FALSE }, // CTS_FORMAT_R32G32B32_SINT
    { GL_RGB_INTEGER,     GL_UNSIGNED_INT,                   GL_RGB32UI,            3, GL_TRUE  }, // CTS_FORMAT_R32G32B32_USCALED
    { GL_RGB_INTEGER,     GL_INT,                            GL_RGB32I,             3, GL_TRUE  }, // CTS_FORMAT_R32G32B32_SSCALED
    { GL_RGB,             GL_FLOAT,                          GL_RGB32F,             3, GL_FALSE }, // CTS_FORMAT_R32G32B32_SFLOAT

    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT,                   GL_RGBA32UI,           4, GL_FALSE }, // CTS_FORMAT_R32G32B32A32_UINT
    { GL_RGBA_INTEGER,    GL_INT,                            GL_RGBA32I,            4, GL_FALSE }, // CTS_FORMAT_R32G32B32A32_SINT
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT,                   GL_RGBA32UI,           4, GL_TRUE  }, // CTS_FORMAT_R32G32B32A32_USCALED
    { GL_RGBA_INTEGER,    GL_INT,                            GL_RGBA32I,            4, GL_TRUE  }, // CTS_FORMAT_R32G32B32A32_SSCALED
    { GL_RGBA,            GL_FLOAT,                          GL_RGBA32F,            4, GL_FALSE }, // CTS_FORMAT_R32G32B32A32_SFLOAT

    { GL_RGB,             GL_UNSIGNED_INT_10F_11F_11F_REV,   GL_R11F_G11F_B10F,     3, GL_FALSE }, // CTS_FORMAT_B10G11R11_UFLOAT_PACK32,
    { GL_RGB,             GL_UNSIGNED_INT_5_9_9_9_REV,       GL_RGB9_E5,            3, GL_FALSE }, // CTS_FORMAT_E5B9G9R9_UFLOAT_PACK32,

    { GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT,                 GL_DEPTH_COMPONENT16,  1, GL_FALSE }, // CTS_FORMAT_D16_UNORM
    { GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,                   GL_DEPTH_COMPONENT24,  1, GL_FALSE }, // CTS_FORMAT_X8_D24_UNORM_PACK32
    { GL_DEPTH_COMPONENT, GL_FLOAT,                          GL_DEPTH_COMPONENT32F, 1, GL_FALSE }, // CTS_FORMAT_D32_SFLOAT 
    { GL_STENCIL_INDEX,   GL_UNSIGNED_BYTE,                  GL_STENCIL_INDEX8,     1, GL_FALSE }, // CTS_FORMAT_S8_UINT,
    { GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8,              GL_DEPTH24_STENCIL8,   2, GL_FALSE }, // CTS_FORMAT_D24_UNORM_S8_UINT
    { GL_DEPTH_STENCIL,   GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_DEPTH32F_STENCIL8,  2, GL_FALSE }, // CTS_FORMAT_D32_SFLOAT_S8_UINT
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
    true,  // CTS_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
    false, // CTS_BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
    true,  // CTS_BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
    false, // CTS_BORDER_COLOR_INT_OPAQUE_BLACK = 3,
    true,  // CTS_BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
    false, // CTS_BORDER_COLOR_INT_OPAQUE_WHITE = 5,
};

static const float sBorderColorsFloat[][4] = {
    {0.0f, 0.0f, 0.0f, 0.0f}, // CTS_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
    {0.0f, 0.0f, 0.0f, 0.0f}, // CTS_BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
    {0.0f, 0.0f, 0.0f, 1.0f}, // CTS_BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
    {0.0f, 0.0f, 0.0f, 1.0f}, // CTS_BORDER_COLOR_INT_OPAQUE_BLACK = 3,
    {1.0f, 1.0f, 1.0f, 1.0f}, // CTS_BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
    {1.0f, 1.0f, 1.0f, 1.0f}, // CTS_BORDER_COLOR_INT_OPAQUE_WHITE = 5,
};

static const int sBorderColorsInt[][4] = {
    {0, 0, 0, 0}, // CTS_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0,
    {0, 0, 0, 0}, // CTS_BORDER_COLOR_INT_TRANSPARENT_BLACK = 1,
    {0, 0, 0, 1}, // CTS_BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2,
    {0, 0, 0, 1}, // CTS_BORDER_COLOR_INT_OPAQUE_BLACK = 3,
    {1, 1, 1, 1}, // CTS_BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4,
    {1, 1, 1, 1}, // CTS_BORDER_COLOR_INT_OPAQUE_WHITE = 5,
};

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
static_assert(COUNTOF(sImageViewTypes) == NUM_CTS_IMAGE_VIEW_TYPES, "Assertion failure");
static_assert(COUNTOF(sFormats) == NUM_CTS_FORMATS, "Assertion failure");
static_assert(COUNTOF(sBlendFactors) == NUM_CTS_BLEND_FACTORS, "Assertion failure");
static_assert(COUNTOF(sBlendOperations) == NUM_CTS_BLEND_OPS, "Assertion failure");
static_assert(COUNTOF(sDynamicStates) == NUM_CTS_DYNAMIC_STATES, "Assertion failure");
static_assert(COUNTOF(sBorderColorIsFloat) == NUM_CTS_BORDER_COLORS, "Assertion failure");
static_assert(COUNTOF(sBorderColorsFloat) == NUM_CTS_BORDER_COLORS, "Assertion failure");
static_assert(COUNTOF(sBorderColorsInt) == NUM_CTS_BORDER_COLORS, "Assertion failure");

const GLenum parseCompareOp(CtsCompareOp value)
{
    return sCompareOps[value];
}

const GLenum parseStencilOp(CtsStencilOp value)
{
    return sStencilOps[value];
}

const GLuint parseCullMode(CtsCullModeFlags value)
{
    GLuint result = 0;

    if (value & CTS_CULL_MODE_FRONT_BIT) {
        result |= GL_FRONT;
    }

    if (value & CTS_CULL_MODE_BACK_BIT) {
        result |= GL_BACK;
    }

    return result;
}

const GLenum parseFrontFace(CtsFrontFace value)
{
    return sFrontFaces[value];
}

const GLenum parsePrimitiveTopology(CtsPrimitiveTopology value)
{
    return sPrimitiveTopologies[value];
}

const GLenum parsePolygonMode(CtsPolygonMode value)
{
    return sPolygonModes[value];
}

const GLenum parseSamplerAddressMode(CtsSamplerAddressMode value)
{
    return sSamplerAddressModes[value];
}

const GLenum parseMinFilter(CtsFilter filter, CtsSamplerMipmapMode mipmapMode)
{
    return (mipmapMode == CTS_SAMPLER_MIPMAP_MODE_LINEAR)
        ? sMinFiltersLinear[filter]
        : sMinFiltersNearest[filter];
}

const GLenum parseMagFilter(CtsFilter filter, CtsSamplerMipmapMode mipmapMode)
{
    return sMagFilters[filter];
}

const GLenum parseShaderType(CtsShaderStageFlags value)
{
    if (value & CTS_SHADER_STAGE_VERTEX_BIT) {
        return GL_VERTEX_SHADER;
    }

    if (value & CTS_SHADER_STAGE_FRAGMENT_BIT) {
        return GL_FRAGMENT_SHADER;
    }

    if (value & CTS_SHADER_STAGE_GEOMETRY_BIT) {
        return GL_GEOMETRY_SHADER;
    }

    if (value & CTS_SHADER_STAGE_COMPUTE_BIT) {
        return GL_COMPUTE_SHADER;
    }

    //CTS_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
    //CTS_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
    //CTS_SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
    //CTS_SHADER_STAGE_ALL = 0x7FFFFFFF
    
    return GL_INVALID_ENUM;
}

const GLenum parseImageType(CtsImageType value, bool isArray)
{
    return isArray
        ? sImageArrayTypes[value]
        : sImageTypes[value];
}

const GLenum parseImageViewType(CtsImageViewType value)
{
    return sImageViewTypes[value];
}

const GLenum parseBufferUsage(CtsBufferUsageFlags value)
{
    if (value & CTS_BUFFER_USAGE_VERTEX_BUFFER_BIT ||
        value & CTS_BUFFER_USAGE_TRANSFER_SRC_BIT ||
        value & CTS_BUFFER_USAGE_TRANSFER_DST_BIT ||
        value & CTS_BUFFER_USAGE_INDIRECT_BUFFER_BIT
    ) {
        return GL_ARRAY_BUFFER;
    }

    if (value & CTS_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }

    if (value & CTS_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        return GL_UNIFORM_BUFFER;
    }

    if (value & CTS_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) {
        return GL_TEXTURE_BUFFER;
    }

    if (value & CTS_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        return GL_INVALID_ENUM; // GL_SHADER_STORAGE_BUFFER;
    }

    if (value & CTS_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)  {
        return GL_INVALID_ENUM; // GL_SHADER_STORAGE_BUFFER;
    }
 
    return GL_INVALID_ENUM;
}

const GLenum parseBlendFactor(CtsBlendFactor value)
{
    return sBlendFactors[value];
}

const GLenum parseBlendOperation(CtsBlendOp value)
{
    return sBlendOperations[value];
}

const CtsFlags parseDynamicStateFlag(CtsDynamicState value)
{
    return sDynamicStates[value];
}

const CtsFormatData parseFormat(CtsFormat value)
{
    return sFormats[value];
}

const bool isFloatBorderColor(CtsBorderColor borderColor)
{
    return sBorderColorIsFloat[borderColor];
}

const float* parseBorderColorFloat(CtsBorderColor borderColor)
{
    return sBorderColorsFloat[borderColor];
}

const int* parseBorderColorInt(CtsBorderColor borderColor)
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

CtsFlagBit parseDynamicStateFlagBits(const CtsPipelineDynamicStateCreateInfo* pCreateInfo) {
    if (pCreateInfo == NULL) {
        return 0;
    }

    CtsFlagBit dynamicState = 0;
    for (uint32_t i = 0; i < pCreateInfo->dynamicStateCount; ++i) {
        dynamicState |= parseDynamicStateFlag(pCreateInfo->pDynamicStates[i]);
    }

    return dynamicState;
}

#ifdef __cplusplus
}
#endif