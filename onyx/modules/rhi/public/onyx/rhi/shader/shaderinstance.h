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
    ShaderInstance( const GraphicsSystem& api, Reference< Pipeline > pipeline, assets::AssetHandle< Shader > shader );

    ~ShaderInstance() override;

    const Reference< Pipeline >& getPipeline() const { return m_pipeline; }
    DynamicArray< Reference< DescriptorSet > >& getDescriptorSets( uint8_t frameIndex );
    const DynamicArray< Reference< DescriptorSet > >& getDescriptorSets( uint8_t frameIndex ) const;

    bool hasDescriptorSets() const { return m_descriptorSets.empty() == false; }
    bool isCompute() const;

    bool isValid() const;

    void bind( const TextureHandle& texture, const String& bindingName, uint8_t frameIndex );
    void bind( const BufferHandle& buffer, const String& bindingName, uint8_t frameIndex );

    void preDraw( uint8_t frameIndex );

  private:
    Reference< DescriptorSet >& getDescriptorSet( uint8_t frameIndex, uint8_t descriptorSetIndex );
    const Reference< DescriptorSet >& getDescriptorSet( uint8_t frameIndex, uint8_t descriptorSetIndex ) const;

    void onShaderLoaded( assets::AssetHandle< Shader > shader );

  private:
    const GraphicsSystem* m_api = nullptr;

    assets::AssetHandle< Shader > m_shader;
    Reference< Pipeline > m_pipeline;
    // node can rebind descriptor bindings
    InplaceArray< DynamicArray< Reference< DescriptorSet > >, MaxFramesInFlight > m_descriptorSets; // per frame
    HashMap< String, uint8_t > m_bindingIdToDescriptorSet;
};
} // namespace onyx::rhi
