#pragma once

#include <onyx/graphics/graphicstypes.h>
#include <onyx/serialize/serialization.h>

namespace Onyx::Graphics
{
    class RenderPass;
    class Shader;

    struct PipelineProperties
    {
        ~PipelineProperties();

        Rasterization m_Rasterization;
        DepthStencil m_DepthStencil;
        InplaceArray<BlendState, MAX_RENDERPASS_ATTACHMENTS> BlendStates;

        PrimitiveTopology Topology = PrimitiveTopology::Triangle;
        Reference<RenderPass> m_RenderPass;

        Reference<Shader> m_Shader;

        String m_DebugName;
    };

    class Pipeline : public RefCounted
    {
    public:
        Pipeline(const PipelineProperties& properties)
            : m_Properties(properties)
        {}
        const PipelineProperties& GetProperties() const { return m_Properties; }
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
