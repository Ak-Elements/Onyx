#pragma once

#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/buffer.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/pipeline.h>
#include <onyx/rhi/texturestorage.h>
#include <onyx/nodegraph/nodes/node.h>
#include <onyx/graphics/rendergraph/rendergraphresource.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

namespace Onyx::Graphics
{
    class GraphicsSystem;
    class CommandBuffer;
    class RenderGraph;
    struct FrameContext;

    // rename to render graph task
    struct RenderGraphPolicy
    {
        using RenderGraphResourceCache = HashMap<RenderGraphResourceId, RenderGraphResource>;
    public:
        virtual void Init(GraphicsSystem& api, RenderGraphResourceCache& resourceCache) = 0;
        virtual void Shutdown(GraphicsSystem& api) = 0;

        virtual void Compile(GraphicsSystem& api, RenderGraphResourceCache& resourceCache) = 0;

        virtual void BeginFrame(RenderGraphContext& context) = 0;

        virtual void PreRender(RenderGraphContext& context, CommandBuffer& commandBuffer) = 0;
        virtual void Render(RenderGraphContext& context, CommandBuffer& commandBuffer) = 0;
        virtual void PostRender(RenderGraphContext& context, CommandBuffer& commandBuffer) = 0;

        virtual void EndFrame(RenderGraphContext& context) = 0;

        // TODO: Find better names for both. IsEnabled -> should this node be started this frame
        // HasBegunFrame - Has this node started this frame
        virtual bool IsEnabled() = 0;
        virtual bool HasBegunFrame() = 0;

        virtual void OnSwapChainResized(GraphicsSystem& api, RenderGraphResourceCache& resourceCache) = 0;
    };

    class IRenderGraphNode : public NodeGraph::NodeWithPolicy<RenderGraphPolicy>
    {
    public:
        void Init(GraphicsSystem& /*api*/, RenderGraphResourceCache& /*resourceCache*/) override { }
        void Shutdown(GraphicsSystem& /*api*/) override { }
        void Compile(GraphicsSystem& /*api*/, RenderGraphResourceCache& /*resourceCache*/) override { }
        void BeginFrame(RenderGraphContext& /*context*/) override { }
        void PreRender(RenderGraphContext& /*context*/, CommandBuffer& /*commandBuffer*/) override { }
        void Render(RenderGraphContext& /*context*/, CommandBuffer& /*commandBuffer*/) override { }
        void PostRender(RenderGraphContext& /*context*/, CommandBuffer& /*commandBuffer*/) override { }
        void EndFrame(RenderGraphContext& /*context*/) override { }

        bool IsEnabled() override { return false; }
        bool HasBegunFrame() override { return false; }

        void OnSwapChainResized(GraphicsSystem& /*api*/, RenderGraphResourceCache& /*resourceCache*/) override {}
    protected:

        template <typename T>
        T& GetGraphInput(RenderGraph& renderGraph)
        {
            return renderGraph.GetInput<T>();
        }

        template <typename T>
        const T& GetGraphInput(RenderGraph& renderGraph) const
        {
            return renderGraph.GetInput<T>();
        }

        template <typename T>
        void AddGraphInput(RenderGraph& renderGraph)
        {
            renderGraph.AddInput<T>();
        }
    };

    class RenderGraphShaderNode : public IRenderGraphNode 
    {
    public:
        void Init(GraphicsSystem& api, RenderGraphResourceCache& resourceCache) override;
        void Shutdown(GraphicsSystem& api) final;

        void Compile(GraphicsSystem& api, RenderGraphResourceCache& resourceCache) override;

        void BeginFrame(RenderGraphContext& context) override;

        void PreRender(RenderGraphContext& context, CommandBuffer& commandBuffer) final;
        void Render(RenderGraphContext& context, CommandBuffer& commandBuffer) override;
        void PostRender(RenderGraphContext& context, CommandBuffer& commandBuffer) final;

        void EndFrame(RenderGraphContext& context) final;

        bool OnSerialize(Serializer& serializer) const override;
        bool OnDeserialize(const Deserializer& deserializer) override;

        const RenderGraphTextureResourceInfo& GetInputResourceInfo(onyxU32 pinIndex) { return pinIndex >= m_InputAttachmentInfos.size() ? m_InputAttachmentInfos.emplace_back() : m_InputAttachmentInfos[pinIndex]; }
        const RenderGraphTextureResourceInfo& GetOuputResourceInfo(onyxU32 pinIndex) { return pinIndex >= m_OutputAttachmentInfos.size() ? m_OutputAttachmentInfos.emplace_back() : m_OutputAttachmentInfos[pinIndex]; }

        void OnSwapChainResized(GraphicsSystem& /*api*/, RenderGraphResourceCache& /*resourceCache*/) override;

        virtual bool IsComputeTask() const { return false; }

        bool HasBegunFrame() override { return m_HasBegunFrame; }

    protected:
#if ONYX_IS_EDITOR
        bool OnDrawInPropertyGrid(HashMap<Guid64, std::any>& constantPinData) override;
#endif

        void BindResources(ShaderInstanceHandle shaderInstance, const RenderGraphResourceCache& resourceCache, const FrameContext& frameContext);

    protected:
        virtual void OnInit(GraphicsSystem&, RenderGraphResourceCache&) {}
        virtual void OnShutdown(GraphicsSystem&) {}

        virtual void OnBeginFrame(RenderGraphContext&) {}

        virtual void OnPreRender(RenderGraphContext&, CommandBuffer&) {}
        virtual void OnRender(RenderGraphContext&, CommandBuffer&) {}
        virtual void OnPostRender(RenderGraphContext&, CommandBuffer&) {}

        virtual void OnEndFrame(RenderGraphContext&) {}

        void CreateRenderPass(GraphicsSystem& api, RenderGraphResourceCache& resourceCache);
        void UpdateFramebuffer(GraphicsSystem& api, RenderGraphResourceCache& resourceCache);

    protected:
        RenderPassHandle m_RenderPass;
        FramebufferHandle m_Framebuffer;

        DynamicArray<RenderGraphTextureResourceInfo> m_InputAttachmentInfos;

        DynamicArray<RenderGraphTextureResourceInfo> m_OutputAttachmentInfos;
        DynamicArray<RenderGraphBufferResourceInfo> m_OutputBufferInfos;

        bool m_HasBegunFrame = false;
    };

    class RenderGraphFixedShaderNode : public RenderGraphShaderNode
    {
    public:
        void Init(GraphicsSystem& api, RenderGraphResourceCache& resourceCache) final;

        void Compile(GraphicsSystem& api, RenderGraphResourceCache& resourceCache) override;

        void BeginFrame(RenderGraphContext& context) final;
        void Render(RenderGraphContext& context, CommandBuffer& commandBuffer) override;

        bool IsComputeTask() const override { ONYX_ASSERT(m_ShaderInstance.IsValid()); return m_ShaderInstance->IsCompute(); }
        bool IsEnabled() override { return m_ShaderInstance.IsValid(); }

        bool OnSerialize(Serializer& serializer) const override;
        bool OnDeserialize(const Deserializer& deserializer) override;

    private:
#if ONYX_IS_EDITOR
        bool OnDrawInPropertyGrid(HashMap<Guid64, std::any>& constantPinData) override;
#endif
    protected:
        PipelineProperties m_PipelineProperties;
        ShaderInstanceHandle m_ShaderInstance;
    };

}
