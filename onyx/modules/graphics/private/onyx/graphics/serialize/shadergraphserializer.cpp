#include <onyx/graphics/serialize/shadergraphserializer.h>

#include <onyx/graphics/shader/shadermodule.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    template <>
    struct Serialization<Graphics::ShaderGraph>
    {
        static bool Serialize(Serializer& serializer, const Graphics::ShaderGraph& nodeGraphNode)
        {
            return nodeGraphNode.Serialize(serializer);
        }

        static bool Deserialize(const Deserializer& deserializer, Graphics::ShaderGraph& outGraphNode)
        {
            return outGraphNode.Deserialize(deserializer);
        }
    };
}

namespace Onyx::Graphics::ShaderGraphSerializer
{
    bool Serialize(const ShaderGraph& graph, const Assets::AssetMetaData& meta, Serializer& serializer)
    {
        using namespace FileSystem;

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR

        bool success = serializer.Write(graph);
        if (success == false)
        {
            return false;
        }

        // save shader to file
        OnyxFile shaderOutFile(Path::ReplaceExtension(meta.Path, "oshader"));
        FileStream shaderOutStream = shaderOutFile.OpenStream(OpenMode::Write | OpenMode::Text);
        shaderOutStream.WriteRaw(graph.GetShaderCode().data(), graph.GetShaderCode().size());

        return success;
#else
        ONYX_UNUSED(graph);
        ONYX_UNUSED(filePath);
        return true;
#endif

    }

    bool Deserialize(ShaderGraph& graph, GraphicsApi& graphicsApi, Assets::AssetSystem& assetSystem, const Assets::AssetMetaData& meta, const Deserializer& deserializer)
    {
        using namespace FileSystem;

        const Filepath& shaderPath = Path::ReplaceExtension(meta.Path, "oshader");
        ShaderHandle shader = graphicsApi.GetShader(shaderPath);

        // TODO: we need to handle the case that the shader graph is updated but the shader code is outdated
        // so we need to compare the compiled graph to the shader on disk

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        deserializer.Read(graph);

        // TODO: Add shader generator factory here to create unlit shader
        PBRShaderGenerator shaderGenerator;

        if (graph.GenerateShader(shaderGenerator) == false)
            return false;

        const onyxU64 shaderCodeHash = Hash::FNV1aHash<onyxU64>(graph.GetShaderCode(), Hash::FNV1aHash<onyxU64>(shaderPath));
        if ((shader == nullptr) || (shaderCodeHash != shader->GetShaderHash()))
        {
            // Save out shader again and trigger reload
            ONYX_LOG_INFO("Shader is not matching need to generate new");
            // save shader and hot reload
        }
#endif

        if (shader == nullptr)
        {
            ONYX_LOG_ERROR("Failed to compile shader for shader graph.");
            return false;
        }

        // call on changed on nodes to queue dependency loading (e.g. textures)
        NodeGraph::NodeGraph& nodeGraph = graph.GetNodeGraph();
        for (auto& node : (nodeGraph.GetNodes() | std::views::values))
        {
            ShaderGraphNode& shaderGraphNode = static_cast<ShaderGraphNode&>(*node.m_Data);
            shaderGraphNode.OnNodeChanged(assetSystem);
        }

        RenderPassSettings renderPassSettings;
        RenderPassSettings::Subpass& subpass = renderPassSettings.m_SubPasses.Emplace();
        RenderPassSettings::Attachment attachment{};

        attachment.m_Format = Enums::ToIntegral(TextureFormat::RGBA_FLOAT32);
        attachment.m_LoadOp = Enums::ToIntegral(RenderPassSettings::LoadOp::DontCare);
        renderPassSettings.m_Attachments.Add(attachment);

        attachment.m_Format = Enums::ToIntegral(TextureFormat::DEPTH_FLOAT32);
        attachment.m_LoadOp = Enums::ToIntegral(RenderPassSettings::LoadOp::Load);
        renderPassSettings.m_Attachments.Add(attachment);

        subpass.m_AttachmentAccesses.Emplace(RenderPassSettings::AttachmentAccess::RenderTarget);
        subpass.m_AttachmentAccesses.Emplace(RenderPassSettings::AttachmentAccess::DepthReadStencilRead);

        PipelineProperties pipelineProperties;
        pipelineProperties.m_Shader = shader;
        pipelineProperties.m_RenderPass = graphicsApi.GetOrCreateRenderPass(renderPassSettings);
        pipelineProperties.m_DebugName = "Test shader graph";

        pipelineProperties.m_Rasterization.m_CullMode = CullMode::Back;
        pipelineProperties.m_DepthStencil.IsDepthEnabled = true;
        pipelineProperties.m_DepthStencil.IsDepthWriteEnabled = false;
        pipelineProperties.m_DepthStencil.IsStencilEnabled = false;
        pipelineProperties.m_DepthStencil.Compare = CompareOperation::LessOrEqual;

        BlendState& blendState = pipelineProperties.BlendStates.Emplace();
        blendState.SourceColor = Blend::SrcAlpha;
        blendState.DestinationColor = Blend::OneMinusSrcAlpha;
        blendState.ColorOperation = BlendOperation::Add;
        blendState.SourceAlpha = Blend::SrcAlpha;
        blendState.DestinationAlpha = Blend::OneMinusSrcAlpha;
        blendState.AlphaOperation = BlendOperation::Add;

        ShaderEffectHandle& shaderEffect = graph.GetShaderEffect();
        shaderEffect = graphicsApi.CreateShaderEffect(pipelineProperties);

        return true;
    }
}
