#pragma once

#include <onyx/assets/asset.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/serialize/serialization.h>

namespace onyx::rhi
{
    class RenderPass;
    class Shader;

    struct PipelineProperties
    {
        InplaceArray<BlendState, MAX_RENDERPASS_ATTACHMENTS> BlendStates;
        DepthStencil DepthStencil;

        Reference<RenderPass> RenderPass;
        assets::AssetId Shader;
        Rasterization Rasterization;

        PrimitiveTopology Topology = PrimitiveTopology::Triangle;
    };

    class Pipeline : public RefCounted
    {
    public:
        Pipeline(const PipelineProperties& properties)
            : m_Properties(properties)
        {
        }

        ONYX_NO_DISCARD const PipelineProperties& GetProperties() const { return m_Properties; }

    private:
        PipelineProperties m_Properties;
    };
}

namespace onyx
{
    template <>
    struct Serialization<rhi::PipelineProperties>
    {
        static bool serialize(Serializer& serializer, const rhi::PipelineProperties& properties);
        static bool deserialize(const Deserializer& deserializer, rhi::PipelineProperties& outProperties);
    };

    template <>
    struct Serialization<rhi::Rasterization>
    {
        static bool serialize(Serializer& serializer, const rhi::Rasterization& rasterization);
        static bool deserialize(const Deserializer& deserializer, rhi::Rasterization& outRasterization);
    };

    template <>
    struct Serialization<rhi::DepthStencil>
    {
        static bool serialize(Serializer& serializer, const rhi::DepthStencil& depthStencil);
        static bool deserialize(const Deserializer& deserializer, rhi::DepthStencil& outDepthStencil);
    };

    template <>
    struct Serialization<rhi::BlendState>
    {
        static bool serialize(Serializer& serializer, const rhi::BlendState& blendState);
        static bool deserialize(const Deserializer& deserializer, rhi::BlendState& outblendState);
    };
}
