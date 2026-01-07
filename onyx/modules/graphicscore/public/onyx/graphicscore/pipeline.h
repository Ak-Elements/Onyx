#pragma once

#include <onyx/assets/asset.h>
#include <onyx/graphicscore/graphicstypes.h>
#include <onyx/serialize/serialization.h>

namespace Onyx::Graphics
{
    class RenderPass;
    class Shader;

    struct PipelineProperties
    {
        InplaceArray<BlendState, MAX_RENDERPASS_ATTACHMENTS> BlendStates;
        DepthStencil DepthStencil;

        Reference<RenderPass> RenderPass;
        Assets::AssetId Shader;
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

namespace Onyx
{
    template <>
    struct Serialization<Graphics::PipelineProperties>
    {
        static bool Serialize(Serializer& serializer, const Graphics::PipelineProperties& properties);
        static bool Deserialize(const Deserializer& deserializer, Graphics::PipelineProperties& outProperties);
    };

    template <>
    struct Serialization<Graphics::Rasterization>
    {
        static bool Serialize(Serializer& serializer, const Graphics::Rasterization& rasterization);
        static bool Deserialize(const Deserializer& deserializer, Graphics::Rasterization& outRasterization);
    };

    template <>
    struct Serialization<Graphics::DepthStencil>
    {
        static bool Serialize(Serializer& serializer, const Graphics::DepthStencil& depthStencil);
        static bool Deserialize(const Deserializer& deserializer, Graphics::DepthStencil& outDepthStencil);
    };

    template <>
    struct Serialization<Graphics::BlendState>
    {
        static bool Serialize(Serializer& serializer, const Graphics::BlendState& blendState);
        static bool Deserialize(const Deserializer& deserializer, Graphics::BlendState& outblendState);
    };
}
