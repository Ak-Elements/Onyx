#pragma once

namespace onyx::rhi {
inline constexpr uint8_t MAX_FRAMES_IN_FLIGHT = 2;

inline constexpr uint8_t MAX_RENDERPASS_ATTACHMENTS = 8;
inline constexpr uint8_t MAX_SUBPASSES = 8;

inline constexpr uint8_t MAX_DESCRIPTOR_SET_LAYOUTS = 8;

// keep in sync with ShaderStage enum in Graphics::ShaderModule
inline constexpr uint8_t MAX_SHADER_STAGES = 5;

inline constexpr uint8_t BINDLESS_SET = 0;
inline constexpr uint32_t MAX_BINDLESS_RESOURCES = 1024;

enum class ApiType : uint8_t { None, Dx12, Vulkan };

enum class Access : uint32_t {
    None,
    IndirectRead = 1 << 0,
    IndexRead = 1 << 1,
    VertexRead = 1 << 2,
    UniformRead = 1 << 3,
    InputAttachmentRead = 1 << 4,
    ShaderRead = 1 << 5,
    ShaderWrite = 1 << 6,
    ColorAttachmentWrite = 1 << 8,
    DepthStencilRead = 1 << 9,
    DepthStencilWrite = 1 << 10,
    TransferRead = 1 << 11,
    TransferWrite = 1 << 12,
    ConditionalRead = 1 << 20,
};

enum class ImageLayout : uint32_t {
    None,
    General,
    ReadOptimal,
    AttachmentOptimal,
    Present,
    TransferDestination,
    TransferSource,
};

enum class CullMode : uint8_t { None = 0, Front, Back };

enum class FillMode : uint8_t { Wireframe, Solid, Point };

enum class CompareOperation : uint8_t {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always,
    Count
};

enum class StencilOperation : uint8_t {
    Keep,
    Zero,
    Replace,
    IncrementClamp,
    DecrementClamp,
    Invert,
    IncrementWrap,
    DecrementWrap
};

enum class Blend : uint8_t {
    Invalid,
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
    Src1Color,
    OneMinusSrc1Color,
    Src1Alpha,
    OneMinusSrc1Alpha,
};

enum class BlendOperation : uint8_t { Add, Subtract, ReverseSubtract, Min, Max };

enum class ColorChannel : uint8_t {
    Red = 0,
    Green = 1,
    Blue = 2,
    Alpha = 4,
    RGB = Red | Green | Blue,
    RGBA = Red | Green | Blue | Alpha
};

enum class PrimitiveTopology : uint8_t {
    Invalid,
    Point,
    Line,
    LineStrip,
    Triangle,
    TriangleStrip,
    TriangleFan,
    Patch,
    Count
};

struct Rasterization {
    CullMode CullMode = CullMode::None;
    bool IsFrontFacing = false;
    FillMode FillMode = FillMode::Solid;
};

struct StencilOperationState {
    StencilOperation Fail = StencilOperation::Keep;
    StencilOperation Pass = StencilOperation::Keep;
    StencilOperation DepthFail = StencilOperation::Keep;
    CompareOperation Compare = CompareOperation::Always;

    uint32_t CompareMask = 0xff;
    uint32_t WriteMask = 0xff;
    uint32_t Reference = 0xff;
};

struct DepthStencil {
    StencilOperationState Front;
    StencilOperationState Back;
    CompareOperation Compare = CompareOperation::Always;

    bool IsDepthEnabled = false;
    bool IsDepthWriteEnabled = false;
    bool IsStencilEnabled = false;
};

struct BlendState {
    Blend SourceColor = Blend::One;
    Blend DestinationColor = Blend::One;
    BlendOperation ColorOperation = BlendOperation::Add;

    Blend SourceAlpha = Blend::Invalid;
    Blend DestinationAlpha = Blend::Invalid;
    BlendOperation AlphaOperation = BlendOperation::Add;

    ColorChannel ColorWriteMask = ColorChannel::RGBA;

    bool IsBlendEnabled = false;
};

enum class TextureFormat : uint8_t {
    Invalid = 0,
    R_UNORM8,
    R_UINT8,
    R_UINT16,
    R_UINT32,
    R_FLOAT32,
    RG_UNORM8,
    RG_FLOAT16,
    RG_FLOAT32,
    RGB_FLOAT32,
    RGB_UNORM8,
    RGBA_UNORM8,
    BGRA_UNORM8,
    RGBA_FLOAT16,
    RGBA_FLOAT32,
    RGB_UFLOAT32_PACKED_11_11_10,
    SRGB_UNORM8,
    DEPTH_STENCIL_FLOAT32_8UINT,
    DEPTH_UNORM16,
    DEPTH_FLOAT32,
    DEPTH_STENCIL_UNORM24_8UINT,
    DEPTH_STENCIL_UNORM16_8UINT,
    STENCIL_UINT8,
};

enum class ShaderLanguage : uint8_t {
    Invalid,
    GLSL,
    HLSL,
    // SLANG,?
    Count
};

static constexpr uint32_t ShaderCoreVersion = 460;

enum class ShaderStage : uint8_t { Invalid, Vertex, Fragment, Compute, All, Count };

enum class ShaderDataType : uint8_t {
    Bool,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Byte,
    Byte4,
    UByte,
    UByte4,
    Short2,
    Short2Normalized,
    Short4,
    Short4Normalized,
    UInt,
    UInt2,
    UInt3,
    UInt4,
    Count
};

enum class VertexStreamInputRate { Vertex, Instance };

inline uint32_t GetShaderTypeByteSize( TextureFormat format ) {
    // Size is bytes per channel * channel
    switch ( format ) {
    case TextureFormat::R_UINT8:
    case TextureFormat::R_UNORM8:
        return 1;
    case TextureFormat::RG_UNORM8:
        return 1 * 2;
    case TextureFormat::RGB_UNORM8:
        return 1 * 3;
    case TextureFormat::RG_FLOAT32:
        return 4 * 2;
    case TextureFormat::RGB_FLOAT32:
        return 4 * 3;
    case TextureFormat::BGRA_UNORM8:
    case TextureFormat::RGBA_UNORM8:
    case TextureFormat::R_FLOAT32:
        return 1 * 4;
    case TextureFormat::RGBA_FLOAT16:
        return 2 * 4;
    case TextureFormat::RGBA_FLOAT32:
        return 4 * 4;
    default:
        ONYX_ASSERT( false, "Not supported shader data type" );
        return 0;
    }
}

enum class TextureUsage { None = 0, Texture, Attachment, Storage, HostRead };

enum class TextureWrap { None = 0, Clamp, Repeat };

enum class TextureFilter { None = 0, Linear, Nearest, Cubic };

enum class Tiling { Optimal, Linear };

struct MSAAProperties {
    uint8_t m_Samples = 0;
    uint8_t m_Quality = 0;

    bool operator==( const MSAAProperties& anOther ) const {
        return m_Samples == anOther.m_Samples && m_Quality == anOther.m_Quality;
    }
    bool operator!=( const MSAAProperties& anOther ) const { return !( *this == anOther ); }
};

enum class TextureType : uint8_t {
    None = 0,
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
};

enum class TextureOrigin { TopLeft, BottomLeft };

enum class CubeMapFace : uint8_t {
    Positive_X,
    Negative_X,
    Positive_Y,
    Negative_Y,
    Positive_Z,
    Negative_Z,
};

enum class IndexType {
    None,
    uint8,
    uint16,
    uint32,
};

enum class Context : uint8_t { Graphics, Compute, CopyTransfer, Count };

struct Viewport {
    int16_t X;
    int16_t Y;

    int16_t Width;
    int16_t Height;

    float32 MinDepth = 0.0f;
    float32 MaxDepth = 0.0f;
};

using ShaderEffectKey = uint64_t;
} // namespace onyx::rhi

template <>
struct std::formatter< onyx::rhi::ShaderDataType > : std::formatter< std::string > {
    auto format( onyx::rhi::ShaderDataType type, format_context& ctx ) const {
        onyx::StringView typeStr;
        switch ( type ) {
        case onyx::rhi::ShaderDataType::Bool:
            typeStr = "bool";
            break;
        case onyx::rhi::ShaderDataType::Float:
            typeStr = "float";
            break;
        case onyx::rhi::ShaderDataType::Float2:
            typeStr = "vec2";
            break;
        case onyx::rhi::ShaderDataType::Float3:
            typeStr = "vec3";
            break;
        case onyx::rhi::ShaderDataType::Float4:
            typeStr = "vec4";
            break;
        case onyx::rhi::ShaderDataType::Mat3:
            typeStr = "mat3";
            break;
        case onyx::rhi::ShaderDataType::Mat4:
            typeStr = "mat4";
            break;
        case onyx::rhi::ShaderDataType::Byte:
            typeStr = "int";
            break;
        case onyx::rhi::ShaderDataType::Byte4:
            typeStr = "int";
            break;
        case onyx::rhi::ShaderDataType::UByte:
            typeStr = "uint";
            break;
        case onyx::rhi::ShaderDataType::UByte4:
            typeStr = "uint";
            break;
        case onyx::rhi::ShaderDataType::Short2:
            typeStr = "vec2";
            break;
        case onyx::rhi::ShaderDataType::Short2Normalized:
            typeStr = "vec2";
            break;
        case onyx::rhi::ShaderDataType::Short4:
            typeStr = "vec4";
            break;
        case onyx::rhi::ShaderDataType::Short4Normalized:
            typeStr = "vec4";
            break;
        case onyx::rhi::ShaderDataType::UInt:
            typeStr = "uint";
            break;
        case onyx::rhi::ShaderDataType::UInt2:
            typeStr = "uvec2";
            break;
        case onyx::rhi::ShaderDataType::UInt3:
            typeStr = "uvec3";
            break;
        case onyx::rhi::ShaderDataType::UInt4:
            typeStr = "uvec4";
            break;
        case onyx::rhi::ShaderDataType::Count:
            break;
        }

        ONYX_ASSERT( typeStr.empty() == false );
        return std::format_to( ctx.out(), "{}", typeStr );
    }
};
