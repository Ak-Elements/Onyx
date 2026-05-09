#include <onyx/editor/nodegraph/volumeshadergrapheditorcontext.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/rhi/shader/shadercompiler.h>
#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>
#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>

namespace onyx::editor {
VolumeShaderGraphEditorContext::VolumeShaderGraphEditorContext( assets::AssetSystem& assetSystem,
                                                                rhi::GraphicsSystem& graphicsSystem )
    : m_AssetSystem( &assetSystem )
    , m_GraphicsSystem( &graphicsSystem ) {}

bool VolumeShaderGraphEditorContext::compile() {
    ONYX_ASSERT( m_GraphicsSystem != nullptr );

    volume::VolumeShaderGraphGenerator generator;
    bool hasGenerated = m_graph->GenerateShader( generator );
    return hasGenerated && rhi::ShaderCompiler::ValidateCode( *m_GraphicsSystem, m_graph->GetShaderCode() );
}

void VolumeShaderGraphEditorContext::onNodeChanged( const Node& newNode ) {
    graphics::ShaderGraphNode& node = getNodeGraph().getNode< graphics::ShaderGraphNode >( newNode.LocalId );
    node.OnNodeChanged( *m_AssetSystem );
}
} // namespace onyx::editor
