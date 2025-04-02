#include <onyx/graphics/serialize/shadergraphserializer.h>

#include <onyx/filesystem/filestream.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/shader/shadermodule.h>
#include <onyx/nodegraph/shadernodefactory.h>
#include <onyx/nodegraph/nodegraphserializer.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Graphics::ShaderGraphSerializer
{
    bool SerializeJson(const ShaderGraph& graph, const FileSystem::Filepath& filePath)
    {
        using namespace FileSystem;

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR

        JsonValue graphJson;
        bool success = graph.Serialize(graphJson);
        if (success == false)
        {
            return false;
        }

        OnyxFile graphOutFile(filePath);
        graphOutFile.WriteJson(graphJson);

        // save shader to file
        OnyxFile shaderOutFile(Path::ReplaceExtension(filePath, "oshader"));
        FileStream shaderOutStream = shaderOutFile.OpenStream(OpenMode::Write | OpenMode::Text);
        shaderOutStream.WriteRaw(graph.GetShaderCode().data(), graph.GetShaderCode().size());

        return success;
#else
        ONYX_UNUSED(asset);
        ONYX_UNUSED(filePath);
        return true;
#endif

    }

    bool DeserializeJson(ShaderGraph& graph, GraphicsApi& graphicsApi, Assets::AssetSystem& assetSystem, const FileSystem::Filepath& filePath)
    {
        using namespace FileSystem;

        const Filepath& shaderPath = Path::ReplaceExtension(filePath, "oshader");
        ShaderHandle shader = graphicsApi.GetShader(shaderPath);

        // TODO: we need to handle the case that the shader graph is updated but the shader code is outdated
        // so we need to compare the compiled graph to the shader on disk

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR
        NodeGraph::ShaderNodeFactory factory;
        OnyxFile graphJsonFile(filePath);
        JsonValue graphJson = graphJsonFile.LoadJson();
        if (graph.Deserialize(graphJson) == false)
        {
            return false;
        }

        // TODO: Add shader generator factory here to create unlit shader
        PBRShaderGenerator shaderGenerator;

        if (graph.GenerateShader(shaderGenerator) == false)
            return false;

        // call on changed on nodes to queue dependency loading (e.g. textures)
        NodeGraph::NodeGraph& nodeGraph = graph.GetNodeGraph();
        for (auto& node : (nodeGraph.GetNodes() | std::views::values))
        {
            ShaderGraphNode& shaderGraphNode = static_cast<ShaderGraphNode&>(*node.m_Data);
            shaderGraphNode.OnNodeChanged(assetSystem);
        }

        const onyxU64 shaderCodeHash = Hash::FNV1aHash64(graph.GetShaderCode(), Hash::FNV1aHash64(shaderPath));
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
        pipelineProperties.Shader = shader;
        pipelineProperties.RenderPass = graphicsApi.GetOrCreateRenderPass(renderPassSettings);
        pipelineProperties.m_DebugName = "Test shader graph";

        pipelineProperties.Rasterization.CullMode = CullMode::Back;
        pipelineProperties.DepthStencil.IsDepthEnabled = true;
        pipelineProperties.DepthStencil.IsDepthWriteEnabled = false;
        pipelineProperties.DepthStencil.IsStencilEnabled = false;
        pipelineProperties.DepthStencil.Compare = CompareOperation::Equal;

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
