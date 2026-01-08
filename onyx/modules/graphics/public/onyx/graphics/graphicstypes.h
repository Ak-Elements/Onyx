#pragma once

namespace Onyx::Graphics
{
    inline constexpr onyxU8 MAX_FRAMES_IN_FLIGHT = 3;

    inline constexpr onyxU8 MAX_RENDERPASS_ATTACHMENTS = 8;
    inline constexpr onyxU8 MAX_SUBPASSES = 8;

    inline constexpr onyxU8 MAX_DESCRIPTOR_SET_LAYOUTS = 8;

    inline constexpr onyxU8 BINDLESS_SET = 0;
    inline constexpr onyxU32 MAX_BINDLESS_RESOURCES = 1024;

    enum class ApiType : onyxU8
    {
        None,
        Dx12,
        Vulkan
    };

    enum class Access : onyxU32
    {
        None,
        IndirectRead = 1 << 0,
        IndexRead = 1 << 1,
        VertexRead = 1 << 2,
        UniformRead = 1 << 3,
        InputAttachmentRead = 1 << 4,
        ShaderRead = 1 << 5,
        ShaderWrite = 1 << 6,
        ConditionalRead = 0x00100000,
    };

    enum class CullMode : onyxU8
    {
        None = 0,
        Front,
        Back
    };

    enum class FillMode : onyxU8
    {
        Wireframe,
        Solid,
        Point
    };

    enum class CompareOperation : onyxU8
    {
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

    enum class StencilOperation : onyxU8
    {
        Keep,
        Zero,
        Replace,
        IncrementClamp,
        DecrementClamp,
        Invert,
        IncrementWrap,
        DecrementWrap
    };

    enum class Blend : onyxU8
    {
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

    enum class BlendOperation : onyxU8
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class ColorChannel : onyxU8
    {
        Red = 0,
        Green = 1,
        Blue = 2,
        Alpha = 4,
        RGB = Red | Green | Blue,
        RGBA = Red | Green | Blue | Alpha
    };

    enum class PrimitiveTopology : onyxU8
    {
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

    struct Rasterization
    {
        CullMode CullMode = CullMode::None;
        bool IsFrontFacing = false;
        FillMode FillMode = FillMode::Solid;
    };

    struct StencilOperationState
    {
        StencilOperation Fail = StencilOperation::Keep;
        StencilOperation Pass = StencilOperation::Keep;
        StencilOperation DepthFail = StencilOperation::Keep;
        CompareOperation Compare = CompareOperation::Always;

        onyxU32 CompareMask = 0xff;
        onyxU32 WriteMask = 0xff;
        onyxU32 Reference = 0xff;
    };

    struct DepthStencil
    {
        StencilOperationState Front;
        StencilOperationState Back;
        CompareOperation Compare = CompareOperation::Always;

        bool IsDepthEnabled = false;
        bool IsDepthWriteEnabled = false;
        bool IsStencilEnabled = false;
    };

    struct BlendState
    {
        Blend SourceColor = Blend::One;
        Blend DestinationColor = Blend::One;
        BlendOperation ColorOperation = BlendOperation::Add;

        Blend SourceAlpha = Blend::Invalid;
        Blend DestinationAlpha = Blend::Invalid;
        BlendOperation AlphaOperation = BlendOperation::Add;

        ColorChannel ColorWriteMask = ColorChannel::RGBA;

        bool IsBlendEnabled = false;
    };

    enum class TextureFormat : onyxU8
    {
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

    enum class ShaderLanguage : onyxU8
    {
        Invalid,
        GLSL,
        HLSL,
        //SLANG,?
        Count
    };

    static constexpr onyxU32 ShaderCoreVersion = 460;

    enum class ShaderStage : onyxU8
    {
        Invalid,
        Vertex,
        Fragment,
        Compute,
        Task,
        Mesh,
        //RayTracing
        All,
        Count
    };

    // keep in sync with ShaderStage enum in Graphics::ShaderModule
    inline constexpr onyxU8 MAX_SHADER_STAGES = static_cast<onyxU8>(ShaderStage::Count);

    enum class ShaderDataType : onyxU8
    {
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

    enum class VertexStreamInputRate
    {
        Vertex,
        Instance
    };

    inline onyxU32 GetShaderTypeByteSize(TextureFormat format)
    {
        // Size is bytes per channel * channel
        switch (format)
        {
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
            ONYX_ASSERT(false, "Not supported shader data type");
            return 0;
        }
    }

    enum class TextureUsage
    {
        None = 0,
        Texture,
        Attachment,
        Storage,
        HostRead
    };

    enum class TextureWrap
    {
        None = 0,
        Clamp,
        Repeat
    };

    enum class TextureFilter
    {
        None = 0,
        Linear,
        Nearest,
        Cubic
    };

    enum class Tiling
    {
        Optimal,
        Linear
    };

    struct MSAAProperties
    {
        onyxU8 m_Samples = 0;
        onyxU8 m_Quality = 0;

        bool operator==(const MSAAProperties& anOther) const { return m_Samples == anOther.m_Samples && m_Quality == anOther.m_Quality; }
        bool operator!=(const MSAAProperties& anOther) const { return !(*this == anOther); }
    };

    enum class TextureType : onyxU8
    {
        None = 0,
        Texture1D,
        Texture2D,
        Texture3D,
        TextureCube,
    };

    enum class TextureOrigin
    {
        TopLeft,
        BottomLeft
    };

    enum class CubeMapFace : onyxU8
    {
        Positive_X,
        Negative_X,
        Positive_Y,
        Negative_Y,
        Positive_Z,
        Negative_Z,
    };

    enum class IndexType
    {
        None,
        uint8,
        uint16,
        uint32,
    };

    enum class Context : onyxU8
    {
        Graphics,
        Compute,
        CopyTransfer,
        Count
    };

    struct Viewport
    {
        onyxS16 X;
        onyxS16 Y;

        onyxS16 Width;
        onyxS16 Height;

        onyxF32 MinDepth = 0.0f;
        onyxF32 MaxDepth = 0.0f;
    };

    using ShaderEffectKey = onyxU64;
}

template <>
struct std::formatter<Onyx::Graphics::ShaderDataType> : std::formatter<std::string>
{
    auto format(Onyx::Graphics::ShaderDataType type, format_context& ctx) const
    {
        Onyx::StringView typeStr;
        switch (type)
        {
            case Onyx::Graphics::ShaderDataType::Bool: typeStr = "bool"; break;
            case Onyx::Graphics::ShaderDataType::Float: typeStr = "float"; break;
            case Onyx::Graphics::ShaderDataType::Float2: typeStr = "vec2"; break;
            case Onyx::Graphics::ShaderDataType::Float3: typeStr = "vec3"; break;
            case Onyx::Graphics::ShaderDataType::Float4: typeStr = "vec4"; break;
            case Onyx::Graphics::ShaderDataType::Mat3: typeStr = "mat3"; break;
            case Onyx::Graphics::ShaderDataType::Mat4: typeStr = "mat4"; break;
            case Onyx::Graphics::ShaderDataType::Byte: typeStr = "int"; break;
            case Onyx::Graphics::ShaderDataType::Byte4: typeStr = "int"; break;
            case Onyx::Graphics::ShaderDataType::UByte: typeStr = "uint"; break;
            case Onyx::Graphics::ShaderDataType::UByte4: typeStr = "uint"; break;
            case Onyx::Graphics::ShaderDataType::Short2: typeStr = "vec2"; break;
            case Onyx::Graphics::ShaderDataType::Short2Normalized: typeStr = "vec2"; break;
            case Onyx::Graphics::ShaderDataType::Short4: typeStr = "vec4"; break;
            case Onyx::Graphics::ShaderDataType::Short4Normalized: typeStr = "vec4"; break;
            case Onyx::Graphics::ShaderDataType::UInt: typeStr = "uint"; break;
            case Onyx::Graphics::ShaderDataType::UInt2: typeStr = "uvec2"; break;
            case Onyx::Graphics::ShaderDataType::UInt3: typeStr = "uvec3"; break;
            case Onyx::Graphics::ShaderDataType::UInt4: typeStr = "uvec4"; break;
            case Onyx::Graphics::ShaderDataType::Count: break;
        }

        ONYX_ASSERT(typeStr.empty() == false);
        return std::format_to(ctx.out(), "{}", typeStr);

    }
};
