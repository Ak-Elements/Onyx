#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/materialshadergraphserializer.h>

#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Graphics
{
    MaterialShaderGraphSerializer::MaterialShaderGraphSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi)
        : AssetSerializer(assetSystem)
        , graphicsApi(graphicsApi)
    {
    }

    bool MaterialShaderGraphSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const
    {
        const MaterialShaderGraph& shaderGraph = asset.As<MaterialShaderGraph>();
        if (ShaderGraphSerializer::Serialize(shaderGraph, serializer) == false)
            return false;

        // save shader to file
        FileSystem::OnyxFile shaderOutFile(FileSystem::Path::GetFullPath(FileSystem::Path::ReplaceExtension(meta.Path, "oshader")));
        FileSystem::FileStream shaderOutStream = shaderOutFile.OpenStream(FileSystem::OpenMode::Write | FileSystem::OpenMode::Text);
        shaderOutStream.WriteRaw(shaderGraph.GetShaderCode().data(), shaderGraph.GetShaderCode().size());

        return true;
    }

    bool MaterialShaderGraphSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const
    {
        MaterialShaderGraph& shaderGraph = asset.As<MaterialShaderGraph>();

        if (ShaderGraphSerializer::Deserialize(shaderGraph, deserializer) == false)
            return false;

        const FileSystem::Filepath& shaderPath = FileSystem::Path::ReplaceExtension(meta.Path, "oshader");
        ShaderHandle shader = graphicsApi.GetShader(shaderPath);

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        // TODO: Add shader generator factory here to create unlit shader
        PBRShaderGenerator shaderGenerator;
        if (shaderGraph.GenerateShader(shaderGenerator) == false)
            return false;

        const onyxU64 shaderCodeHash = Hash::FNV1aHash<onyxU64>(shaderGraph.GetShaderCode(), Hash::FNV1aHash<onyxU64>(shaderPath));
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
        NodeGraph::NodeGraph& nodeGraph = shaderGraph.GetNodeGraph();
        for (auto& node : (nodeGraph.GetNodes() | std::views::values))
        {
            ShaderGraphNode& shaderGraphNode = static_cast<ShaderGraphNode&>(*node.m_Data);
            shaderGraphNode.OnNodeChanged(m_AssetSystem);
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

        ShaderEffectHandle& shaderEffect = shaderGraph.GetShaderEffect();
        shaderEffect = graphicsApi.CreateShaderEffect(pipelineProperties);
        return true;
    }

}
