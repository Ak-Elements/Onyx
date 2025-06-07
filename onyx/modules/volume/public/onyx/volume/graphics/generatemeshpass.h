#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Volume
{
    class CreateVolumeMesh : public NodeGraph::FixedPinNode_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle>
    {
    public:
        CreateVolumeMesh();

        static constexpr StringId32 TypeId = "Onyx::Volume::RenderGraph::CreateVolumeMesh";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        void OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;

        void OnBeginFrame(const Graphics::RenderGraphContext& context) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

    private:
        InplaceArray<Graphics::BufferHandle, 1> m_VoxelGrid;
    };

    class GenerateVolumeMesh : public NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::BufferHandle>
    {
    public:
        GenerateVolumeMesh();

        static constexpr StringId32 TypeId = "Onyx::Volume::RenderGraph::GenerateVolumeMesh";
        StringId32 GetTypeId() const override { return TypeId; }

        static Graphics::BufferHandle GetVertexBuffer() { return m_VertexBuffer[0]; }

    private:
        void OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;

        void OnBeginFrame(const Graphics::RenderGraphContext& context) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

    private:
        static InplaceArray<Graphics::BufferHandle, 1> m_VertexBuffer;
        InplaceArray<Graphics::BufferHandle, 1> m_VertexCountBuffer;
    };

}
