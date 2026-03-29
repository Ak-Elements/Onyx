#include <onyx/graphics/serialize/materialshadergraphserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/pipeline.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics {
bool MaterialShaderGraphSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                               const assets::AssetMetaData& meta,
                                               Serializer& serializer,
                                               const IEngine& /*engine*/ ) const {
    const MaterialShaderGraph& shaderGraph = asset.as< MaterialShaderGraph >();
    if ( shader_graph_serializer::serialize( shaderGraph, serializer ) == false )
        return false;

    // save shader to file
    file_system::OnyxFile shaderOutFile(
        file_system::path::getFullPath( file_system::path::replaceExtension( meta.Path, "oshader" ) ) );
    file_system::FileStream shaderOutStream = shaderOutFile.OpenStream( file_system::OpenMode::Write |
                                                                        file_system::OpenMode::Text );
    shaderOutStream.writeRaw( shaderGraph.GetShaderCode().data(), shaderGraph.GetShaderCode().size() );

    return true;
}

bool MaterialShaderGraphSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                                 const assets::AssetMetaData& meta,
                                                 const Deserializer& deserializer,
                                                 IEngine& engine ) const {
    rhi::GraphicsSystem& graphicsSystem = engine.getSystem< rhi::GraphicsSystem >();
    assets::AssetSystem& assetSystem = engine.getSystem< assets::AssetSystem >();

    MaterialShaderGraph& shaderGraph = asset.as< MaterialShaderGraph >();

    if ( shader_graph_serializer::deserialize( shaderGraph, deserializer ) == false )
        return false;

    const FilePath& shaderPath = file_system::path::replaceExtension( meta.Path, "oshader" );
    // ShaderHandle shader = graphicsApi.CreateShader();
    // graphicsApi.GetShaderCache().GetOrLoadShader(shaderPath, shader);

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
    // TODO: Add shader generator factory here to create unlit shader
    rhi::PBRShaderGenerator shaderGenerator;
    if ( shaderGraph.GenerateShader( shaderGenerator ) == false )
        return false;

    // const uint64_t shaderCodeHash = hash::FNV1aHash<uint64_t>(shaderGraph.GetShaderCode(),
    // hash::FNV1aHash<uint64_t>(shaderPath)); if ((shader == nullptr) || (shaderCodeHash != shader->GetShaderHash()))
    //{
    //     // Save out shader again and trigger reload
    //     ONYX_LOG_INFO("Shader is not matching need to generate new");
    //     // save shader and hot reload
    // }
#endif

    /*if (shader == nullptr)
    {
        ONYX_LOG_ERROR("Failed to compile shader for shader graph.");
        return false;
    }*/

    // call on changed on nodes to queue dependency loading (e.g. textures)
    node_graph::NodeGraph& nodeGraph = shaderGraph.GetNodeGraph();
    for ( auto& node : ( nodeGraph.GetNodes() | std::views::values ) ) {
        ShaderGraphNode& shaderGraphNode = static_cast< ShaderGraphNode& >( *node.Data );
        shaderGraphNode.OnNodeChanged( assetSystem );
    }

    rhi::RenderPassSettings renderPassSettings;
    rhi::RenderPassSettings::Subpass& subpass = renderPassSettings.m_SubPasses.emplace();
    rhi::RenderPassSettings::Attachment attachment{};

    attachment.m_Format = enums::toIntegral( rhi::TextureFormat::RGBA_FLOAT32 );
    attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::DontCare );
    renderPassSettings.m_Attachments.add( attachment );

    attachment.m_Format = enums::toIntegral( rhi::TextureFormat::DEPTH_FLOAT32 );
    attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::Load );
    renderPassSettings.m_Attachments.add( attachment );

    subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::RenderTarget );
    subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::DepthReadStencilRead );

    rhi::PipelineProperties pipelineProperties;
    pipelineProperties.Shader = assets::AssetId( shaderPath );
    pipelineProperties.RenderPass = graphicsSystem.GetOrCreateRenderPass( renderPassSettings );

    pipelineProperties.Rasterization.CullMode = rhi::CullMode::Back;
    pipelineProperties.DepthStencil.IsDepthEnabled = true;
    pipelineProperties.DepthStencil.IsDepthWriteEnabled = false;
    pipelineProperties.DepthStencil.IsStencilEnabled = false;
    pipelineProperties.DepthStencil.Compare = rhi::CompareOperation::Equal;

    rhi::BlendState& blendState = pipelineProperties.BlendStates.emplace();
    blendState.SourceColor = rhi::Blend::SrcAlpha;
    blendState.DestinationColor = rhi::Blend::OneMinusSrcAlpha;
    blendState.ColorOperation = rhi::BlendOperation::Add;
    blendState.SourceAlpha = rhi::Blend::SrcAlpha;
    blendState.DestinationAlpha = rhi::Blend::OneMinusSrcAlpha;
    blendState.AlphaOperation = rhi::BlendOperation::Add;

    rhi::ShaderInstanceHandle& shaderEffect = shaderGraph.GetShader();
    shaderEffect = graphicsSystem.CreateShaderInstance( assets::AssetId( shaderPath ), pipelineProperties );

    return true;
}

} // namespace onyx::graphics
