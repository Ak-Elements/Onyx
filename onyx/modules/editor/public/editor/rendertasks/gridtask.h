#pragma once

#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx
{
    class GridRenderGraphNode : public NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle>
    {
        using Super = NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::TextureHandle>;

    private:
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case Super::InPin::LocalId: return "ViewConstants";
            case Super::OutPin::LocalId: return "GridTexture";
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }
#endif

    private:
#if ONYX_IS_DEBUG
        String m_Name;
#endif

        Graphics::Context m_ContextType = Graphics::Context::Count;
        //RenderGraphTaskParameters m_Parameters;

        //UniquePtr<Graphics::RenderGraphTask> m_Task;
        DynamicArray<Graphics::RenderGraphResourceId> m_Resources;

        //TODO: Remove for non shader tasks e.g.: ViewConstants / Environment Constants / Scene data updates 
        Graphics::PipelineProperties m_PipelineProperties;
        Graphics::ShaderHandle m_Shader;
        Graphics::ShaderEffectHandle m_ShaderEffect;

        Graphics::RenderPassHandle m_RenderPass;
        Graphics::FramebufferHandle m_Framebuffer;
    };
}
