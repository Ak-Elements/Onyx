#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1out.h>
#include <onyx/nodegraph/nodes/fixedpinnode1in1out.h>
#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Volume
{
    class CreateVolumeMesh : public NodeGraph::FixedPinNode_1_Out<Graphics::RenderGraphShaderNode, Graphics::BufferHandle>
    {
    public:
        CreateVolumeMesh();

        static constexpr StringId32 TypeId = "Onyx::Volume::RenderGraph::CreateVolumeMesh";
        StringId32 GetTypeId() const override { return TypeId; }

        bool IsEnabled() override { return true; }
        bool IsComputeTask() const override { return true; }

    private:
        void OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;
       
        void OnBeginFrame(const Graphics::RenderGraphContext& context) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;

    private:
        Graphics::ShaderHandle m_CreateTerrainShader;
        Graphics::ShaderEffectHandle m_CreateTerrainShaderEffect;

        Graphics::ShaderHandle m_GenerateMeshShader;
        Graphics::ShaderEffectHandle m_GenerateMeshShaderEffect;
    };

    class GenerateVolumeMesh : public NodeGraph::FixedPinNode_1_In_1_Out<Graphics::RenderGraphFixedShaderNode, Graphics::BufferHandle, Graphics::BufferHandle>
    {
    public:
        GenerateVolumeMesh();
        
        static constexpr StringId32 TypeId = "Onyx::Volume::RenderGraph::GenerateVolumeMesh";
        StringId32 GetTypeId() const override { return TypeId; }

        //static Graphics::BufferHandle GetVertexCountBuffer() { return m_VertexCountBuffer[0]; }

        bool IsEnabled() override { return false; }

        static void SetModified(bool modified);
    private:
        void OnInit(Graphics::GraphicsApi& api, RenderGraphResourceCache& resourceCache) override;

        void OnBeginFrame(const Graphics::RenderGraphContext& context) override;
        void OnRender(Graphics::RenderGraphContext& context, Graphics::CommandBuffer& commandBuffer) override;
    };

}
