#pragma once

#include <onyx/graphics/graphicstypes.h>

namespace Onyx::Graphics
{
    class RenderPass;
    class Shader;

    struct PipelineProperties
    {
        Rasterization Rasterization;
        DepthStencil DepthStencil;
        InplaceArray<BlendState, MAX_RENDERPASS_ATTACHMENTS> BlendStates;

        PrimitiveTopology Topology = PrimitiveTopology::Triangle;
        Reference<RenderPass> RenderPass;

        Reference<Shader> Shader;

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