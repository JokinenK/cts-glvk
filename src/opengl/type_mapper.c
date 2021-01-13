#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <cts/typedefs/gl_enums.h>
#include <cts/type_mapper.h>

#ifndef COUNTOF
#  define COUNTOF(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

static const float EPSILON = 1.0f / 1000.0f;

static const GLenum sCompareOperators[] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

static const GLenum sStencilActions[] = {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_INCR_WRAP,
    GL_DECR,
    GL_DECR_WRAP,
    GL_INVERT
};

static const GLenum sCullModes[] = {
    GL_NONE,
    GL_FRONT,
    GL_BACK,
    GL_FRONT_AND_BACK
};

static const GLenum sFrontFaces[] = {
    GL_CW,
    GL_CCW
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

static const GLint sFilters[] = {
    GL_NEAREST,
    GL_LINEAR
};

static const CtsAttributeMapping sAttributeMappings[] = {
    { GL_BYTE,              sizeof(GLbyte),       1   }, // Int8
    { GL_SHORT,             sizeof(GLshort),      1   }, // Int16
    { GL_INT,               sizeof(GLint),        1   }, // Int32
    { GL_UNSIGNED_BYTE,     sizeof(GLubyte),      1   }, // UInt8
    { GL_UNSIGNED_SHORT,    sizeof(GLushort),     1   }, // UInt16
    { GL_UNSIGNED_INT,      sizeof(GLuint),       1   }, // UInt32
    { GL_FLOAT,             sizeof(GLfloat),      1   }, // Float
    { GL_DOUBLE,            sizeof(GLdouble),     1   }, // Double
    { GL_BOOL,              sizeof(GLboolean),    1   }, // Bool
    { GL_UNSIGNED_INT_24_8, sizeof(GLuint),       1   }, // Uint24_8
    { GL_FLOAT_VEC2,        sizeof(GLfloat[2]),   2   }, // Vec2
    { GL_FLOAT_VEC3,        sizeof(GLfloat[3]),   3   }, // Vec3
    { GL_FLOAT_VEC4,        sizeof(GLfloat[4]),   4   }, // Vec4
    { GL_INT_VEC2,          sizeof(GLint[2]),     2   }, // IVec2
    { GL_INT_VEC3,          sizeof(GLint[3]),     3   }, // IVec3
    { GL_INT_VEC4,          sizeof(GLint[4]),     4   }, // IVec4
    { GL_UNSIGNED_INT_VEC2, sizeof(GLuint[2]),    2   }, // UVec2
    { GL_UNSIGNED_INT_VEC3, sizeof(GLuint[3]),    3   }, // UVec3
    { GL_UNSIGNED_INT_VEC4, sizeof(GLuint[4]),    4   }, // UVec4
    { GL_BOOL_VEC2,         sizeof(GLboolean[2]), 2   }, // BVec2
    { GL_BOOL_VEC3,         sizeof(GLboolean[3]), 3   }, // BVec3
    { GL_BOOL_VEC4,         sizeof(GLboolean[4]), 4   }, // BVec4
    { GL_FLOAT_MAT2,        sizeof(GLfloat[2*2]), 2*2 }, // Mat2
    { GL_FLOAT_MAT3,        sizeof(GLfloat[3*3]), 3*3 }, // Mat3
    { GL_FLOAT_MAT4,        sizeof(GLfloat[4*4]), 4*4 }, // Mat4
    { GL_SAMPLER_1D,        sizeof(GLuint),       1   }, // Sampler1D
    { GL_SAMPLER_2D,        sizeof(GLuint),       1   }, // Sampler2D
    { GL_SAMPLER_3D,        sizeof(GLuint),       1   }, // Sampler3D
    { GL_SAMPLER_CUBE,      sizeof(GLuint),       1   }  // SamplerCube
};

static GLenum sIndexTypes[] = {
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_INT
};

static GLenum sShaderTypes[] = {
    GL_FRAGMENT_SHADER,
    GL_VERTEX_SHADER,
    GL_GEOMETRY_SHADER
};

static GLenum sImageTypes[] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D
};

static const CtsFormatData sFormats[] = {
    { GL_INVALID_ENUM,    GL_INVALID_ENUM,                   GL_INVALID_ENUM       }, // CTS_FORMAT_UNDEFINED
      
    { GL_RGBA,            GL_UNSIGNED_SHORT_4_4_4_4,         GL_RGBA4              }, // CTS_FORMAT_R4G4B4A4_UNORM_PACK16
    { GL_BGRA,            GL_UNSIGNED_SHORT_4_4_4_4_REV,     GL_RGBA4              }, // CTS_FORMAT_B4G4R4A4_UNORM_PACK16
                  
    { GL_RGBA,            GL_UNSIGNED_SHORT_5_5_5_1,         GL_RGB5_A1            }, // CTS_FORMAT_R5G5B5A1_UNORM_PACK16
    { GL_BGRA,            GL_UNSIGNED_SHORT_5_5_5_1,         GL_RGB5_A1            }, // CTS_FORMAT_B5G5R5A1_UNORM_PACK16
                  
    { GL_RED,             GL_UNSIGNED_BYTE,                  GL_R8                 }, // CTS_FORMAT_R8_UNORM
    { GL_RED,             GL_BYTE,                           GL_R8_SNORM           }, // CTS_FORMAT_R8_SNORM 
    { GL_RED_INTEGER,     GL_UNSIGNED_BYTE,                  GL_R8UI               }, // CTS_FORMAT_R8_UINT 
    { GL_RED_INTEGER,     GL_BYTE,                           GL_R8I                }, // CTS_FORMAT_R8_SINT 
                  
    { GL_RG,              GL_UNSIGNED_BYTE,                  GL_RG8                }, // CTS_FORMAT_R8G8_UNORM 
    { GL_RG,              GL_BYTE,                           GL_RG8_SNORM          }, // CTS_FORMAT_R8G8_SNORM 
    { GL_RG_INTEGER,      GL_UNSIGNED_BYTE,                  GL_RG8UI              }, // CTS_FORMAT_R8G8_UINT 
    { GL_RG_INTEGER,      GL_BYTE,                           GL_RG8I               }, // CTS_FORMAT_R8G8_SINT 
            
    { GL_RGB,             GL_UNSIGNED_BYTE,                  GL_RGB8               }, // CTS_FORMAT_R8G8B8_UNORM 
    { GL_RGB,             GL_BYTE,                           GL_RGB8_SNORM         }, // CTS_FORMAT_R8G8B8_SNORM 
    { GL_RGB_INTEGER,     GL_UNSIGNED_BYTE,                  GL_RGB8UI             }, // CTS_FORMAT_R8G8B8_UINT 
    { GL_RGB_INTEGER,     GL_BYTE,                           GL_RGB8I              }, // CTS_FORMAT_R8G8B8_SINT 
    { GL_RGB,             GL_BYTE,                           GL_SRGB8              }, // CTS_FORMAT_R8G8B8_SRGB 
                  
    { GL_BGR,             GL_UNSIGNED_BYTE,                  GL_RGB8               }, // CTS_FORMAT_B8G8R8_UNORM 
    { GL_BGR,             GL_BYTE,                           GL_RGB8_SNORM         }, // CTS_FORMAT_B8G8R8_SNORM 
    { GL_BGR_INTEGER,     GL_UNSIGNED_BYTE,                  GL_RGB8UI             }, // CTS_FORMAT_B8G8R8_UINT 
    { GL_BGR_INTEGER,     GL_BYTE,                           GL_RGB8I              }, // CTS_FORMAT_B8G8R8_SINT 
                 
    { GL_RGBA,            GL_UNSIGNED_BYTE,                  GL_RGBA8              }, // CTS_FORMAT_R8G8B8A8_UNORM 
    { GL_RGBA,            GL_BYTE,                           GL_RGBA8_SNORM        }, // CTS_FORMAT_R8G8B8A8_SNORM 
    { GL_RGBA_INTEGER,    GL_UNSIGNED_BYTE,                  GL_RGBA8UI            }, // CTS_FORMAT_R8G8B8A8_UINT 
    { GL_RGBA_INTEGER,    GL_BYTE,                           GL_RGBA8I             }, // CTS_FORMAT_R8G8B8A8_SINT 
    { GL_RGBA,            GL_BYTE,                           GL_SRGB8_ALPHA8       }, // CTS_FORMAT_R8G8B8A8_SRGB 
                 
    { GL_BGRA,            GL_UNSIGNED_BYTE,                  GL_RGBA8              }, // CTS_FORMAT_B8G8R8A8_UNORM 
    { GL_BGRA,            GL_BYTE,                           GL_RGBA8_SNORM        }, // CTS_FORMAT_B8G8R8A8_SNORM 
    { GL_BGRA_INTEGER,    GL_UNSIGNED_BYTE,                  GL_RGBA8UI            }, // CTS_FORMAT_B8G8R8A8_UINT 
    { GL_BGRA_INTEGER,    GL_BYTE,                           GL_RGBA8I             }, // CTS_FORMAT_B8G8R8A8_SINT 

    { GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8              }, // CTS_FORMAT_A8B8G8R8_UNORM_PACK32,
    { GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8_SNORM        }, // CTS_FORMAT_A8B8G8R8_SNORM_PACK32,
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8UI            }, // CTS_FORMAT_A8B8G8R8_UINT_PACK32,
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT_8_8_8_8_REV,       GL_RGBA8I             }, // CTS_FORMAT_A8B8G8R8_SINT_PACK32,
    { GL_RGBA,            GL_UNSIGNED_INT_8_8_8_8_REV,       GL_SRGB8_ALPHA8       }, // CTS_FORMAT_A8B8G8R8_SRGB_PACK32,

    { GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2UI         }, // CTS_FORMAT_A2B10G10R10_UNORM_PACK32,
    { GL_RGBA,            GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2           }, // CTS_FORMAT_A2B10G10R10_SNORM_PACK32,
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2UI         }, // CTS_FORMAT_A2B10G10R10_UINT_PACK32,
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT_2_10_10_10_REV,    GL_RGB10_A2           }, // CTS_FORMAT_A2B10G10R10_SINT_PACK32,

    { GL_RED,             GL_UNSIGNED_SHORT,                 GL_R16                }, // CTS_FORMAT_R16_UNORM 
    { GL_RED,             GL_SHORT,                          GL_R16_SNORM          }, // CTS_FORMAT_R16_SNORM 
    { GL_RED_INTEGER,     GL_UNSIGNED_SHORT,                 GL_R16UI              }, // CTS_FORMAT_R16_UINT 
    { GL_RED_INTEGER,     GL_SHORT,                          GL_R16I               }, // CTS_FORMAT_R16_SINT 
    { GL_RED,             GL_FLOAT,                          GL_R16F               }, // CTS_FORMAT_R16_SFLOAT 
                           
    { GL_RG,              GL_UNSIGNED_SHORT,                 GL_RG16               }, // CTS_FORMAT_R16G16_UNORM 
    { GL_RG,              GL_SHORT,                          GL_RG16_SNORM         }, // CTS_FORMAT_R16G16_SNORM 
    { GL_RG_INTEGER,      GL_UNSIGNED_SHORT,                 GL_RG16UI             }, // CTS_FORMAT_R16G16_UINT 
    { GL_RG_INTEGER,      GL_SHORT,                          GL_RG16I              }, // CTS_FORMAT_R16G16_SINT 
    { GL_RG,              GL_FLOAT,                          GL_RG16F              }, // CTS_FORMAT_R16G16_SFLOAT 
                          
    { GL_RGB,             GL_UNSIGNED_SHORT,                 GL_RGB16              }, // CTS_FORMAT_R16G16B16_UNORM 
    { GL_RGB,             GL_SHORT,                          GL_RGB16_SNORM        }, // CTS_FORMAT_R16G16B16_SNORM 
    { GL_RGB_INTEGER,     GL_UNSIGNED_SHORT,                 GL_RGB16UI            }, // CTS_FORMAT_R16G16B16_UINT 
    { GL_RGB_INTEGER,     GL_SHORT,                          GL_RGB16I             }, // CTS_FORMAT_R16G16B16_SINT 
    { GL_RGB,             GL_FLOAT,                          GL_RGB16F             }, // CTS_FORMAT_R16G16B16_SFLOAT 
                         
    { GL_RGBA,            GL_UNSIGNED_SHORT,                 GL_RGBA16             }, // CTS_FORMAT_R16G16B16A16_UNORM 
    { GL_RGBA,            GL_SHORT,                          GL_RGBA16_SNORM       }, // CTS_FORMAT_R16G16B16A16_SNORM 
    { GL_RGBA_INTEGER,    GL_UNSIGNED_SHORT,                 GL_RGBA16UI           }, // CTS_FORMAT_R16G16B16A16_UINT 
    { GL_RGBA_INTEGER,    GL_SHORT,                          GL_RGBA16I            }, // CTS_FORMAT_R16G16B16A16_SINT 
    { GL_RGBA,            GL_HALF_FLOAT,                     GL_RGBA16F            }, // CTS_FORMAT_R16G16B16A16_SFLOAT 
                       
    { GL_RED_INTEGER,     GL_UNSIGNED_INT,                   GL_R32UI              }, // CTS_FORMAT_R32_UINT 
    { GL_RED_INTEGER,     GL_INT,                            GL_R32I               }, // CTS_FORMAT_R32_SINT 
    { GL_RED,             GL_FLOAT,                          GL_R32F               }, // CTS_FORMAT_R32_SFLOAT
                                
    { GL_RG_INTEGER,      GL_UNSIGNED_INT,                   GL_RG32UI             }, // CTS_FORMAT_R32G32_UINT
    { GL_RG_INTEGER,      GL_INT,                            GL_RG32I              }, // CTS_FORMAT_R32G32_SINT
    { GL_RG,              GL_FLOAT,                          GL_RG32F              }, // CTS_FORMAT_R32G32_SFLOAT
                                 
    { GL_RGB_INTEGER,     GL_UNSIGNED_INT,                   GL_RGB32UI            }, // CTS_FORMAT_R32G32B32_UINT
    { GL_RGB_INTEGER,     GL_INT,                            GL_RGB32I             }, // CTS_FORMAT_R32G32B32_SINT
    { GL_RGB,             GL_FLOAT,                          GL_RGB32F             }, // CTS_FORMAT_R32G32B32_SFLOAT
                               
    { GL_RGBA_INTEGER,    GL_UNSIGNED_INT,                   GL_RGBA32UI           }, // CTS_FORMAT_R32G32B32A32_UINT
    { GL_RGBA_INTEGER,    GL_INT,                            GL_RGBA32I            }, // CTS_FORMAT_R32G32B32A32_SINT
    { GL_RGBA,            GL_FLOAT,                          GL_RGBA32F            }, // CTS_FORMAT_R32G32B32A32_SFLOAT
                
    { GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT,                 GL_DEPTH_COMPONENT16  }, // CTS_FORMAT_D16_UNORM
    { GL_DEPTH_COMPONENT, GL_FLOAT,                          GL_DEPTH_COMPONENT32F }, // CTS_FORMAT_D32_SFLOAT

    { GL_DEPTH_STENCIL,   GL_UNSIGNED_INT_24_8,              GL_DEPTH24_STENCIL8   }, // CTS_FORMAT_D24_UNORM_S8_UINT
    { GL_DEPTH_STENCIL,   GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_DEPTH32F_STENCIL8  }, // CTS_FORMAT_D32_SFLOAT_S8_UINT
};

static GLenum sPrimitiveTypes[] = {
    GL_POINTS,         // Point,
    GL_LINES,          // Line,
    GL_LINE_LOOP,      // LineLoop,
    GL_LINE_STRIP,     // LineStrip,
    GL_TRIANGLES,      // Triangle,
    GL_TRIANGLE_STRIP, // TriangleStrip,
    GL_TRIANGLE_FAN,   // TriangleFan
};

static GLenum sBufferUsages[] = {
    GL_STATIC_DRAW,    // StaticDraw,
    GL_DYNAMIC_DRAW,   // DynamicDraw,
    GL_STREAM_DRAW,    // StreamDraw,
};

static const GLenum sBufferTypes[] = {
    GL_ELEMENT_ARRAY_BUFFER, // IndexBuffer,
    GL_ARRAY_BUFFER,         // VertexBuffer,
    GL_UNIFORM_BUFFER,       // UniformBuffer,
};

static const GLenum sBlendFuncs[] = {
    GL_ZERO,                     // Zero,
    GL_ONE,                      // One,
    GL_SRC_COLOR,                // SrcColor,
    GL_ONE_MINUS_SRC_COLOR,      // OneMinusSrcColor,
    GL_DST_COLOR,                // DstColor,
    GL_ONE_MINUS_DST_COLOR,      // OneMinusDstColor,
    GL_SRC_ALPHA,                // SrcAlpha,
    GL_ONE_MINUS_SRC_ALPHA,      // OneMinusSrcAlpha,
    GL_DST_ALPHA,                // DstAlpha,
    GL_ONE_MINUS_DST_ALPHA,      // OneMinusDstAlpha,
    GL_CONSTANT_COLOR,           // ConstantColor,
    GL_ONE_MINUS_CONSTANT_COLOR, // OneMinusConstantColor,
    GL_CONSTANT_ALPHA,           // ConstantAlpha,
    GL_ONE_MINUS_CONSTANT_ALPHA, // OneMinusConstantAlpha,
    GL_SRC_ALPHA_SATURATE,       // SrcAlphaSaturate,
};

static const GLenum sBlendOperations[] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX
};

static const GLenum sRenderTargetAttachments[] = {
    GL_COLOR_ATTACHMENT0,  // ColorAttachment0
    GL_COLOR_ATTACHMENT1,  // ColorAttachment1
    GL_COLOR_ATTACHMENT2,  // ColorAttachment2
    GL_COLOR_ATTACHMENT3,  // ColorAttachment3
    GL_COLOR_ATTACHMENT4,  // ColorAttachment4
    GL_COLOR_ATTACHMENT5,  // ColorAttachment5
    GL_COLOR_ATTACHMENT6,  // ColorAttachment6
    GL_COLOR_ATTACHMENT7,  // ColorAttachment7
    GL_COLOR_ATTACHMENT8,  // ColorAttachment8
    GL_COLOR_ATTACHMENT9,  // ColorAttachment9
    GL_COLOR_ATTACHMENT10, // ColorAttachment10
    GL_COLOR_ATTACHMENT11, // ColorAttachment11
    GL_COLOR_ATTACHMENT12, // ColorAttachment12
    GL_COLOR_ATTACHMENT13, // ColorAttachment13
    GL_COLOR_ATTACHMENT14, // ColorAttachment14
    GL_COLOR_ATTACHMENT15, // ColorAttachment15
    GL_COLOR_ATTACHMENT16, // ColorAttachment16
    GL_COLOR_ATTACHMENT17, // ColorAttachment17
    GL_COLOR_ATTACHMENT18, // ColorAttachment18
    GL_COLOR_ATTACHMENT19, // ColorAttachment19
    GL_COLOR_ATTACHMENT20, // ColorAttachment20
    GL_COLOR_ATTACHMENT21, // ColorAttachment21
    GL_COLOR_ATTACHMENT22, // ColorAttachment22
    GL_COLOR_ATTACHMENT23, // ColorAttachment23
    GL_COLOR_ATTACHMENT24, // ColorAttachment24
    GL_COLOR_ATTACHMENT25, // ColorAttachment25
    GL_COLOR_ATTACHMENT26, // ColorAttachment26
    GL_COLOR_ATTACHMENT27, // ColorAttachment27
    GL_COLOR_ATTACHMENT28, // ColorAttachment28
    GL_COLOR_ATTACHMENT29, // ColorAttachment29
    GL_COLOR_ATTACHMENT30, // ColorAttachment30
    GL_COLOR_ATTACHMENT31, // ColorAttachment31
    GL_DEPTH_ATTACHMENT,   // DepthAttachment
    GL_STENCIL_ATTACHMENT, // StencilAttachment
};

static const GLenum sRenderTargetBinds[] = {
    GL_READ_FRAMEBUFFER,
    GL_DRAW_FRAMEBUFFER,
    GL_FRAMEBUFFER,
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

static_assert(COUNTOF(sCompareOperators) == NUM_CTS_COMPARE_OPERATORS, "Assertion failure");
static_assert(COUNTOF(sStencilActions) == NUM_CTS_STENCIL_ACTIONS, "Assertion failure");
static_assert(COUNTOF(sCullModes) == NUM_CTS_CULL_MODES, "Assertion failure");
static_assert(COUNTOF(sFrontFaces) == NUM_CTS_FRONT_FACES, "Assertion failure");
static_assert(COUNTOF(sPolygonModes) == NUM_CTS_POLYGON_MODES, "Assertion failure");
static_assert(COUNTOF(sSamplerAddressModes) == NUM_CTS_SAMPLER_ADDRESS_MODES, "Assertion failure");
static_assert(COUNTOF(sFilters) == NUM_CTS_FILTERS, "Assertion failure");
static_assert(COUNTOF(sAttributeMappings) == NUM_CTS_ATTRIB_TYPES, "Assertion failure");
static_assert(COUNTOF(sShaderTypes) == NUM_CTS_SHADER_TYPES, "Assertion failure");
static_assert(COUNTOF(sImageTypes) == NUM_CTS_IMAGE_TYPES, "Assertion failure");
static_assert(COUNTOF(sFormats) == NUM_CTS_FORMATS, "Assertion failure");
static_assert(COUNTOF(sPrimitiveTypes) == NUM_CTS_PRIMITIVE_TYPES, "Assertion failure");
static_assert(COUNTOF(sBufferUsages) == NUM_CTS_BUFFER_USAGES, "Assertion failure");
static_assert(COUNTOF(sBufferTypes) == NUM_CTS_BUFFER_TYPES, "Assertion failure");
static_assert(COUNTOF(sBlendFuncs) == NUM_CTS_BLEND_FUNCS, "Assertion failure");
static_assert(COUNTOF(sBlendOperations) == NUM_CTS_BLEND_OPS, "Assertion failure");
static_assert(COUNTOF(sRenderTargetAttachments) == NUM_CTS_RENDER_TARGET_ATTACHMENTS, "Assertion failure");
static_assert(COUNTOF(sRenderTargetBinds) == NUM_CTS_RENDER_TARGET_BINDS, "Assertion failure");
static_assert(COUNTOF(sDynamicStates) == NUM_CTS_DYNAMIC_STATES, "Assertion failure");

const GLenum parseCompareOperator(CtsCompareOperator pValue)
{
    return sCompareOperators[pValue];
}

const GLenum parseStencilAction(CtsStencilAction pValue)
{
    return sStencilActions[pValue];
}

const GLenum parseCullMode(CtsCullMode pValue)
{
    return sCullModes[pValue];
}

const GLenum parseFrontFace(CtsFrontFace pValue)
{
    return sFrontFaces[pValue];
}

const GLenum parsePolygonMode(CtsPolygonMode pValue)
{
    return sPolygonModes[pValue];
}

const GLenum parseSamplerAddressMode(CtsSamplerAddressMode pValue)
{
    return sSamplerAddressModes[pValue];
}

const GLenum parseFilter(CtsFilter pValue)
{
    return sFilters[pValue];
}

const GLenum parseIndexType(CtsIndexType pValue)
{
    return sIndexTypes[pValue];
}

const GLenum parseShaderType(CtsShaderType pValue)
{
    return sShaderTypes[pValue];
}

const GLenum parseImageType(CtsImageType pValue)
{
    return sImageTypes[pValue];
}

const GLenum parsePrimitiveType(CtsPrimitiveType pValue)
{
    return sPrimitiveTypes[pValue];
}

const GLenum parseBufferUsage(CtsBufferUsage pValue)
{
    return sBufferUsages[pValue];
}

const GLenum parseBufferType(CtsBufferType pValue)
{
    return sBufferTypes[pValue];
}

const GLenum parseBlendFunc(CtsBlendFunc pValue)
{
    return sBlendFuncs[pValue];
}

const GLenum parseBlendOperation(CtsBlendOp pValue)
{
    return sBlendOperations[pValue];
}

const GLenum parseRenderTargetAttachment(CtsRenderTargetAttachment pValue)
{
    return sRenderTargetAttachments[pValue];
}

const GLenum parseRenderTargetBind(CtsRenderTargetBind pValue)
{
    return sRenderTargetBinds[pValue];
}

const CtsFlags parseDynamicStateFlag(CtsDynamicState pValue)
{
    return sDynamicStates[pValue];
}

const CtsFormatData parseFormat(CtsFormat pValue)
{
    return sFormats[pValue];
}

const CtsAttributeMapping parseAttributeMapping(CtsAttribType pValue)
{
    return sAttributeMappings[pValue];
}

const CtsGlDescriptorType parseDescriptorType(CtsDescriptorType pValue)
{
    switch (pValue) {
        case CTS_DESCRIPTOR_TYPE_SAMPLER:
        case CTS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        case CTS_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case CTS_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        case CTS_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            return CTS_GL_DESCRIPTOR_TYPE_IMAGE_INFO;

        case CTS_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
        case CTS_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            return CTS_GL_DESCRIPTOR_TYPE_BUFFER_VIEW;

        case CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case CTS_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        case CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
        case CTS_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            return CTS_GL_DESCRIPTOR_TYPE_BUFFER_INFO;
        
        default:
            return NUM_CTS_GL_DESCRIPTOR_TYPES;
    }
}

const CtsAttribType parseAttribType(GLenum pType)
{
    switch (pType) {
        case GL_BYTE:              return CTS_ATTRIB_TYPE_INT8;
        case GL_SHORT:             return CTS_ATTRIB_TYPE_INT16;
        case GL_INT:               return CTS_ATTRIB_TYPE_INT32;

        case GL_UNSIGNED_BYTE:     return CTS_ATTRIB_TYPE_UINT8;
        case GL_UNSIGNED_SHORT:    return CTS_ATTRIB_TYPE_UINT16;
        case GL_UNSIGNED_INT:      return CTS_ATTRIB_TYPE_UINT32;

        case GL_FLOAT:             return CTS_ATTRIB_TYPE_FLOAT;
        case GL_DOUBLE:            return CTS_ATTRIB_TYPE_DOUBLE;
        case GL_BOOL:              return CTS_ATTRIB_TYPE_BOOL;

        case GL_UNSIGNED_INT_24_8: return CTS_ATTRIB_TYPE_UINT24_8;

        case GL_FLOAT_VEC2:        return CTS_ATTRIB_TYPE_VEC2;
        case GL_FLOAT_VEC3:        return CTS_ATTRIB_TYPE_VEC3;
        case GL_FLOAT_VEC4:        return CTS_ATTRIB_TYPE_VEC4;

        case GL_INT_VEC2:          return CTS_ATTRIB_TYPE_IVEC2;
        case GL_INT_VEC3:          return CTS_ATTRIB_TYPE_IVEC3;
        case GL_INT_VEC4:          return CTS_ATTRIB_TYPE_IVEC4;

        case GL_UNSIGNED_INT_VEC2: return CTS_ATTRIB_TYPE_UVEC2;
        case GL_UNSIGNED_INT_VEC3: return CTS_ATTRIB_TYPE_UVEC3;
        case GL_UNSIGNED_INT_VEC4: return CTS_ATTRIB_TYPE_UVEC4;
        
        case GL_BOOL_VEC2:         return CTS_ATTRIB_TYPE_BVEC2;
        case GL_BOOL_VEC3:         return CTS_ATTRIB_TYPE_BVEC3;
        case GL_BOOL_VEC4:         return CTS_ATTRIB_TYPE_BVEC4;

        case GL_FLOAT_MAT2:        return CTS_ATTRIB_TYPE_MAT2;
        case GL_FLOAT_MAT3:        return CTS_ATTRIB_TYPE_MAT3;
        case GL_FLOAT_MAT4:        return CTS_ATTRIB_TYPE_MAT4;

        case GL_SAMPLER_1D:        return CTS_ATTRIB_TYPE_SAMPLER_1D;
        case GL_SAMPLER_2D:        return CTS_ATTRIB_TYPE_SAMPLER_2D;
        case GL_SAMPLER_3D:        return CTS_ATTRIB_TYPE_SAMPLER_3D;
        case GL_SAMPLER_CUBE:      return CTS_ATTRIB_TYPE_SAMPLER_CUBE;
    }

    return NUM_CTS_ATTRIB_TYPES;
}


void parseRasterizationStateChanges(
    const CtsGlPipelineRasterizationState* pLhs, 
    const CtsGlPipelineRasterizationState* pRhs, 
    CtsGlRasterizationStateChanges* pChanges
) {
    pChanges->depthClampEnableChanged        = pLhs->depthClampEnable        != pRhs->depthClampEnable;
    pChanges->rasterizerDiscardEnableChanged = pLhs->rasterizerDiscardEnable != pRhs->rasterizerDiscardEnable;
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
    pChanges->depthTestEnableChanged       = pLhs->depthTestEnable       != pRhs->depthTestEnable;
    pChanges->depthWriteEnableChanged      = pLhs->depthWriteEnable      != pRhs->depthWriteEnable;
    pChanges->depthCompareOpChanged        = pLhs->depthCompareOp        != pRhs->depthCompareOp;
    pChanges->depthBoundsTestEnableChanged = pLhs->depthBoundsTestEnable != pRhs->depthBoundsTestEnable;
    pChanges->stencilTestEnableChanged     = pLhs->stencilTestEnable     != pRhs->stencilTestEnable;

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

bool parseColorBlendStateBlendConstantChanged(
    CtsGlPipelineColorBlendState* pLhs,
    CtsGlPipelineColorBlendState* pRhs
) {
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
    pChanges->blendEnableChanged    = pLhs->blendEnable != pRhs->blendEnable;
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
        dynamicState |= parseDynamicStateFlag(pCreateInfo->dynamicStates[i]);
    }

    return dynamicState;
}

#ifdef __cplusplus
}
#endif