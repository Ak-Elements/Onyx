#pragma once

#include <onyx/assets/asset.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/serialize/serialization.h>

#include <utility>

namespace onyx::rhi {
class RenderPass;
class Shader;

struct PipelineProperties {
    InplaceArray< BlendState, MaxRenderpassAttachments > BlendStates;
    DepthStencil DepthStencil;

    Reference< RenderPass > RenderPass;
    assets::AssetId Shader;
    Rasterization Rasterization;

    PrimitiveTopology Topology = PrimitiveTopology::Triangle;
};

class Pipeline : public RefCounted {
  public:
    Pipeline( PipelineProperties properties )
        : m_properties( std::move( properties ) ) {}

    [[nodiscard]] const PipelineProperties& getProperties() const { return m_properties; }

  private:
    PipelineProperties m_properties;
};
} // namespace onyx::rhi

namespace onyx {
template <>
struct Serialization< rhi::PipelineProperties > {
    static bool serialize( Serializer& serializer, const rhi::PipelineProperties& properties );
    static bool deserialize( const Deserializer& deserializer, rhi::PipelineProperties& outProperties );
};

template <>
struct Serialization< rhi::Rasterization > {
    static bool serialize( Serializer& serializer, const rhi::Rasterization& rasterization );
    static bool deserialize( const Deserializer& deserializer, rhi::Rasterization& outRasterization );
};

template <>
struct Serialization< rhi::DepthStencil > {
    static bool serialize( Serializer& serializer, const rhi::DepthStencil& depthStencil );
    static bool deserialize( const Deserializer& deserializer, rhi::DepthStencil& outDepthStencil );
};

template <>
struct Serialization< rhi::BlendState > {
    static bool serialize( Serializer& serializer, const rhi::BlendState& blendState );
    static bool deserialize( const Deserializer& deserializer, rhi::BlendState& outblendState );
};
} // namespace onyx
