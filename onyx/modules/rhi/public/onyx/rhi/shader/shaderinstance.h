#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/shader/shader.h>

namespace onyx::rhi {
class Buffer;
class Pipeline;
class Shader;
class ShaderCache;
class GraphicsSystem;
class DescriptorSet;

struct BufferHandle;
struct TextureHandle;
struct PipelineProperties;

class ShaderInstance : public RefCounted {
  public:
    ShaderInstance() = default;
    ShaderInstance( const GraphicsSystem& api,
                    const Reference< Pipeline >& pipeline,
                    const assets::AssetHandle< Shader >& shader );

    const Reference< Pipeline >& GetPipeline() const { return m_Pipeline; }
    DynamicArray< Reference< DescriptorSet > >& GetDescriptorSets( uint8_t frameIndex );
    const DynamicArray< Reference< DescriptorSet > >& GetDescriptorSets( uint8_t frameIndex ) const;

    bool HasDescriptorSets() const { return m_DescriptorSets.empty() == false; }
    bool IsCompute() const;

    bool IsValid() const;

    void Bind( const TextureHandle& texture, const String& bindingName, uint8_t frameIndex );
    void Bind( const BufferHandle& buffer, const String& bindingName, uint8_t frameIndex );

    void PreDraw( uint8_t frameIndex );

  private:
    Reference< DescriptorSet >& GetDescriptorSet( uint8_t frameIndex, uint8_t descriptorSetIndex );
    const Reference< DescriptorSet >& GetDescriptorSet( uint8_t frameIndex, uint8_t descriptorSetIndex ) const;

    void OnShaderLoaded( assets::AssetHandle< Shader > shader );

  private:
    const GraphicsSystem* m_Api = nullptr;

    assets::AssetHandle< Shader > m_Shader;
    Reference< Pipeline > m_Pipeline;
    // node can rebind descriptor bindings
    InplaceArray< DynamicArray< Reference< DescriptorSet > >, MAX_FRAMES_IN_FLIGHT > m_DescriptorSets; // per frame
    HashMap< String, uint8_t > m_BindingIdToDescriptorSet;
};
} // namespace onyx::rhi
