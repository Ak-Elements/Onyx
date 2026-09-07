#pragma once

#include <onyx/onyx_types.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/string/format.h>

namespace onyx::rhi {

struct ShaderVariable {
    StringId32 Name;
    ShaderDataType Type;
    ShaderSemantic Semantic;
    uint8_t SemanticIndex = 0;
};

struct ShaderPushConstant {
    StringId32 Name;
    ShaderDataType Type;
    String SubType;
};

struct ShaderTexture {
    uint64_t Id;
};

class ShaderGenerator {
  public:
    struct StageGenerationContext {
        ShaderStage Stage;
        String Code;

        DynamicArray< ShaderVariable > Inputs;
        DynamicArray< ShaderVariable > Outputs;

        Vector3u8 Threads;
    };

  public:
    ShaderGenerator() = default;
    ShaderGenerator( ShaderStage stage );

    virtual ~ShaderGenerator() = default;

    int32_t addTexture( uint64_t textureId ) {
        int32_t index = getTextureIndex( textureId );
        if( index != InvalidIndex32 ) {
            return index;
        }

        index = static_cast< int32_t >( m_textures.size() );
        m_textures.emplace_back( textureId );
        return index;
    }

    int32_t getTextureIndex( uint64_t textureId ) {
        auto it = std::ranges::find_if( m_textures,
                                        [ & ]( const ShaderTexture& texture ) { return texture.Id == textureId; } );
        if( it == m_textures.end() ) {
            return InvalidIndex32;
        }

        return static_cast< int32_t >( std::distance( m_textures.begin(), it ) );
    }

    void setCurrentStage( ShaderStage stage ) { m_currentStage = stage; }
    ShaderStage getCurrentStage() const { return m_currentStage; }

    void addInput( ShaderVariable input );
    void addInput( ShaderStage stage, ShaderVariable input );
    void addOutput( ShaderVariable input );
    void addOutput( ShaderStage stage, ShaderVariable input );

    void appendCode( StringView code );

    bool hasPushConstant( StringId32 name ) const;

    void addPushConstant( ShaderPushConstant constant );

    void addImport( String include );
    void addInclude( String include );

    String generateShader();

    template < typename T >
    static String generateShaderValue( const T& value ) {
        // TODO: Add uint2, uint3 and uint4 support
        if constexpr( is_specialization_of_v< Vector4, T > ) {
            return String( format::format( "float4({}, {}, {}, {})", value.X, value.Y, value.Z, value.W ) );
        } else if constexpr( is_specialization_of_v< Vector3, T > ) {
            return String( format::format( "float3({}, {}, {})", value.X, value.Y, value.Z ) );
        } else if constexpr( is_specialization_of_v< Vector2, T > ) {
            return String( format::format( "float2({}, {})", value.X, value.Y ) );
        } else if constexpr( std::is_integral_v< T > || std::is_floating_point_v< T > ) {
            return std::to_string( value );
        } else
            return "";
    }

    template < typename T >
    static String getTypeAsShaderTypeString() {
        if constexpr( is_specialization_of_v< Vector4, T > ) {
            return "float4";
        } else if constexpr( is_specialization_of_v< Vector3, T > ) {
            return "float3";
        } else if constexpr( is_specialization_of_v< Vector2, T > ) {
            return "float2";
        } else if constexpr( std::is_floating_point_v< T > ) {
            return "float";
        } else if constexpr( std::is_integral_v< T > ) {
            return std::is_signed_v< T > ? "int" : "uint";
        } else
            return "";
    }

    static StringView shaderTypeToString( ShaderDataType type ) {
        switch( type ) {
        case ShaderDataType::Bool:
            return "bool";
        case ShaderDataType::Float:
            return "float";
        case ShaderDataType::Float2:
            return "float2";
        case ShaderDataType::Float3:
            return "float3";
        case ShaderDataType::Float4:
            return "float4";
        case ShaderDataType::Mat3:
            return "float3x3";
        case ShaderDataType::Mat4:
            return "float4x4";
        case ShaderDataType::Byte:
            return "int8_t";
        case ShaderDataType::Byte4:
            return "vector<int8_t, 4>";
        case ShaderDataType::UByte:
            return "uint8_t";
        case ShaderDataType::UByte4:
            return "vector<uint8_t, 4>";
        case ShaderDataType::Short2:
            return "vector<int16_t, 2>";
        case ShaderDataType::Short4:
            return "vector<int16_t, 4>";
        case ShaderDataType::UInt:
            return "uint32_t";
        case ShaderDataType::UInt2:
            return "uint2";
        case ShaderDataType::UInt3:
            return "uint3";
        case ShaderDataType::UInt4:
            return "uint4";
        case onyx::rhi::ShaderDataType::UInt64:
            return "uint64_t";
        case onyx::rhi::ShaderDataType::Pointer:
            return "Ptr";
        case ShaderDataType::Count:
            return "";
            ONYX_ASSERT( false, "Invalid shader data type" );
        }
    }

  private:
    StageGenerationContext& getOrCreateStage( ShaderStage stage );
    Optional< StageGenerationContext* > getStage( ShaderStage stage );

    void generatePushConstants( String& stageCode );
    void generateIncludes( String& stageCode );
    void generateImports( String& stageCode );

    String generateShaderStageCode( const StageGenerationContext& stageContext ) const;

    virtual void generateShaderStage( StageGenerationContext& stageContext ) = 0;

    // TODO: Do not submit and fix shader generator isntead of hacking it like that
  protected:
    DynamicArray< ShaderTexture > m_textures;
    DynamicArray< ShaderPushConstant > m_pushConstants;

    ShaderStage m_currentStage = ShaderStage::Invalid;
    HashSet< String > m_shaderImports;
    HashSet< String > m_shaderIncludes;
    DynamicArray< StageGenerationContext > m_shaderStages;
};

class PBRShaderGenerator : public ShaderGenerator {
  public:
    PBRShaderGenerator();

  protected:
    void generateShaderStage( StageGenerationContext& stageContext ) override;

  private:
    void generateVertexStage();
    void generateFragmentStage();
};
} // namespace onyx::rhi
