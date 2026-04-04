#pragma once

#include <onyx/assets/asset.h>
#include <onyx/rhi/graphicstypes.h>

#include <onyx/stream/stream.h>

namespace onyx {
class IEngine;
}

namespace onyx::rhi {
class GraphicsSystem;

struct UniformBuffer {
    StringId32 Id = 0;
    uint32_t BindingPoint = 0;
    uint32_t Size = 0;
    TextureFormat Format = TextureFormat::Invalid;
    ShaderStage Stage = ShaderStage::Invalid;

    void Serialize( Stream& outStream ) const {
        outStream.write( Id );
        outStream.write( BindingPoint );
        outStream.write( Size );
        outStream.write( Format );
        outStream.write( Stage );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( Id );
        inStream.read( BindingPoint );
        inStream.read( Size );
        inStream.read( Format );
        inStream.read( Stage );
    }
};

struct StorageBuffer {
    StringId32 Id = 0;
    uint32_t BindingPoint = 0;
    uint32_t Size = 0;
    ShaderStage Stage = ShaderStage::Invalid;

    void Serialize( Stream& outStream ) const {
        outStream.write( Id );
        outStream.write( BindingPoint );
        outStream.write( Size );
        outStream.write( Stage );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( Id );
        inStream.read( BindingPoint );
        inStream.read( Size );
        inStream.read( Stage );
    }
};

struct ImageSampler {
    uint32_t BindingPoint = 0;
    uint32_t DescriptorSet = 0;
    uint32_t ArraySize = 0;
    uint32_t Dimension = 0;
    String Name;
    ShaderStage Stage = ShaderStage::Invalid;

    void Serialize( Stream& outStream ) const {
        outStream.write( BindingPoint );
        outStream.write( DescriptorSet );
        outStream.write( ArraySize );
        outStream.write( Dimension );
        outStream.write( Name );
        outStream.write( Stage );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( BindingPoint );
        inStream.read( DescriptorSet );
        inStream.read( ArraySize );
        inStream.read( Dimension );
        inStream.read( Name );
        inStream.read( Stage );
    }
};

struct PushConstantRange {
    ShaderStage Stage = ShaderStage::Invalid;
    uint32_t Offset = 0;
    uint32_t Size = 0;

    void Serialize( Stream& outStream ) const {
        outStream.write( Stage );
        outStream.write( Offset );
        outStream.write( Size );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( Stage );
        inStream.read( Offset );
        inStream.read( Size );
    }
};

struct ShaderUniform {
    String Name;
    uint32_t Size = 0;
    uint32_t Offset = 0;

    void Serialize( Stream& outStream ) const {
        outStream.write( Name );
        outStream.write( Size );
        outStream.write( Offset );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( Name );
        inStream.read( Size );
        inStream.read( Offset );
    }
};

struct ShaderBuffer {
    String Name;
    uint32_t Size = 0;
    HashMap< String, ShaderUniform > Uniforms;

    void Serialize( Stream& outStream ) const {
        outStream.write( Name );
        outStream.write( Size );
        outStream.write( Uniforms );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( Name );
        inStream.read( Size );
        inStream.read( Uniforms );
    }
};

struct ShaderResourceDeclaration {
    String Name;
    uint32_t Set = 0;
    uint32_t Register = 0;
    uint32_t Count = 0;

    void Serialize( Stream& outStream ) const {
        outStream.write( Name );
        outStream.write( Set );
        outStream.write( Register );
        outStream.write( Count );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( Name );
        inStream.read( Set );
        inStream.read( Register );
        inStream.read( Count );
    }
};

struct ShaderDescriptorSet {
    uint8_t Set;

    HashMap< uint32_t, UniformBuffer > UniformBuffers;
    HashMap< uint32_t, StorageBuffer > StorageBuffers;
    HashMap< uint32_t, ImageSampler > ImageSamplers;
    HashMap< uint32_t, ImageSampler > StorageImages;
    HashMap< uint32_t, ImageSampler > SeparateTextures; // Not really an image sampler.
    HashMap< uint32_t, ImageSampler > SeparateSamplers;

    void Serialize( Stream& outStream ) const {
        outStream.write( Set );
        outStream.write( UniformBuffers );
        outStream.write( StorageBuffers );
        outStream.write( ImageSamplers );
        outStream.write( StorageImages );
        outStream.write( SeparateTextures );
        outStream.write( SeparateSamplers );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( Set );
        inStream.read( UniformBuffers );
        inStream.read( StorageBuffers );
        inStream.read( ImageSamplers );
        inStream.read( StorageImages );
        inStream.read( SeparateTextures );
        inStream.read( SeparateSamplers );
    }
};

struct VertexInput {
    TextureFormat Format;
    uint32_t Location;

    bool operator<( const VertexInput& other ) const { return Location < other.Location; }

    void Serialize( Stream& outStream ) const {
        outStream.write( Format );
        outStream.write( Location );
    }

    void Deserialize( const Stream& outStream ) {
        outStream.read( Format );
        outStream.read( Location );
    }
};

struct VertexInputStream {
    uint32_t GetStride() const { return m_Stride; }
    const Set< VertexInput >& GetInputs() const { return m_Inputs; }
    void Add( uint32_t location, TextureFormat format ) { m_Inputs.emplace( format, location ); }

    void Serialize( Stream& outStream ) const {
        outStream.write( m_Stride );
        outStream.write< Set >( m_Inputs );
    }

    void Deserialize( const Stream& outStream ) {
        outStream.read( m_Stride );
        outStream.read( m_Inputs );
    }

  private:
    uint32_t m_Stride = 0;
    Set< VertexInput > m_Inputs;
};

struct ShaderReflectionInfo {
    bool IsUsingBindless = false;

    VertexInputStream vertexInput;
    DynamicArray< ShaderDescriptorSet > shaderDescriptorSets;
    DynamicArray< PushConstantRange > pushConstantRanges;

    // TODO: Do we need the string here or could this be a hash?
    HashMap< String, ShaderResourceDeclaration > shaderResources;
    HashMap< String, ShaderBuffer > constantBuffers;
    HashMap< String, TextureFormat > OutputAttachments;

    void Serialize( Stream& outStream ) const {
        outStream.write( IsUsingBindless );
        outStream.write( vertexInput );
        outStream.write< DynamicArray >( shaderDescriptorSets );
        outStream.write< DynamicArray >( pushConstantRanges );
        outStream.write( shaderResources );
        outStream.write( constantBuffers );
    }

    void Deserialize( const Stream& inStream ) {
        inStream.read( IsUsingBindless );
        inStream.read( vertexInput );
        inStream.read< DynamicArray >( shaderDescriptorSets );
        inStream.read< DynamicArray >( pushConstantRanges );
        inStream.read( shaderResources );
        inStream.read( constantBuffers );
    }
};

class Shader : public assets::Asset< Shader > {
  public:
    static Reference< Shader > create( const IEngine& engine );

    static constexpr StringId32 TypeId{ "onyx::Graphics::assets::Shader" };
    StringId32 GetTypeId() const { return TypeId; }

    using ByteCode = DynamicArray< uint32_t >;
    using PerStageByteCodes = InplaceArray< ByteCode, MAX_SHADER_STAGES >;

    virtual bool AddStage( GraphicsSystem& api, ShaderStage stage, const ByteCode& byteCode ) = 0;
    virtual void RemoveStage( ShaderStage stage ) = 0;

    virtual const ShaderReflectionInfo& GetReflectionData() const = 0;
    virtual bool UpdateReflectionData( GraphicsSystem& api, ShaderReflectionInfo& reflectionInfo ) = 0;

    virtual uint64_t GetShaderHash() const = 0;
    virtual void SetShaderHash( uint64_t hash ) = 0;

    virtual bool IsComputeShader() const = 0;
    virtual bool HasDescriptorSetLayout() const = 0;

#if !ONYX_IS_RETAIL
    virtual StringView getPath() const = 0;
    virtual void setPath( const String& path ) = 0;
#endif
};

DynamicArray< FilePath > GetShaderDirectories();
} // namespace onyx::rhi
