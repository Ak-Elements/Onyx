#pragma once

#include <onyx/onyx_types.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/string/format.h>

namespace onyx::rhi {
struct ShaderVariable {
    String Name;
    ShaderDataType Type;
    uint32_t Offset = 0;
};

struct ShaderVariableHash {
    using IsTransparent = void;
    size_t operator()( const ShaderVariable& variable ) const { return std::hash< String >{}( variable.Name ); }
    size_t operator()( const String& name ) const { return std::hash< String >{}( name ); }
};

struct ShaderTexture {
    uint64_t Id;
};

class ShaderGenerator {
  public:
    ShaderGenerator() = default;
    virtual ~ShaderGenerator() = default;

    template < typename T >
    static String generateShaderValue( const T& value ) {
        if constexpr( is_specialization_of_v< Vector4, T > ) {
            return String( format::format( "vec4({}, {}, {}, {})", value[ 0 ], value[ 1 ], value[ 2 ], value[ 3 ] ) );
        } else if constexpr( is_specialization_of_v< Vector3, T > ) {
            return String( format::format( "vec3({}, {}, {})", value[ 0 ], value[ 1 ], value[ 2 ] ) );
        } else if constexpr( is_specialization_of_v< Vector2, T > ) {
            return String( format::format( "vec2({}, {})", value[ 0 ], value[ 1 ] ) );
        } else if constexpr( std::is_integral_v< T > || std::is_floating_point_v< T > ) {
            return std::to_string( value );
        } else
            return "";
    }

    template < typename T >
    static String getTypeAsShaderTypeString() {
        if constexpr( is_specialization_of_v< Vector4, T > ) {
            return "vec4";
        } else if constexpr( is_specialization_of_v< Vector3, T > ) {
            return "vec3";
        } else if constexpr( is_specialization_of_v< Vector2, T > ) {
            return "vec2";
        } else if constexpr( std::is_floating_point_v< T > ) {
            return "float";
        } else if constexpr( std::is_integral_v< T > ) {
            return std::is_signed_v< T > ? "int" : "uint";
        } else
            return "";
    }

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

    void setStage( ShaderStage stage ) { m_currentStage = stage; }
    ShaderStage getStage() const { return m_currentStage; }

    void appendCode( StringView code );

    bool hasPushConstant( StringView name ) const;
    bool hasPushConstant( ShaderStage stage, StringView name ) const;

    void addPushConstant( StringView name, ShaderDataType type );
    void addPushConstant( ShaderStage stage, StringView name, ShaderDataType type );
    void addPushConstant( ShaderStage stage, StringView name, ShaderDataType type, uint32_t offset );

    void addInclude( String include );

    // TODO: Do not submit and fix shader generator isntead of hacking it like that
    virtual String generateShader();

  private:
    void generateVertexShader();
    void generateFragmentShader();

    void generatePushConstants( String& stageCode );
    void generateIncludes( String& stageCode );

    virtual void doGenerateFragmentMain() {}

    // TODO: Do not submit and fix shader generator isntead of hacking it like that
  protected:
    DynamicArray< ShaderTexture > m_textures;

    InplaceArray< DynamicArray< ShaderVariable >, MaxShaderStages > m_pushConstants;

    DynamicArray< ShaderVariable > m_vertexInputs;
    DynamicArray< ShaderVariable > m_vertexOutputs;

    ShaderStage m_currentStage = ShaderStage::Invalid;
    HashSet< String > m_shaderIncludes;
    InplaceArray< String, MaxShaderStages > m_shaderStagesCode;
};

class PBRShaderGenerator : public ShaderGenerator {
  public:
    PBRShaderGenerator();

  protected:
    void doGenerateFragmentMain() override;
};
} // namespace onyx::rhi
