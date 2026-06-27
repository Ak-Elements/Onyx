#include <onyx/editor/nodegraph/volumeshadergrapheditorcontext.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/rhi/shader/shadercompiler.h>
#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>
#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>

namespace onyx::editor {
VolumeShaderGraphEditorContext::VolumeShaderGraphEditorContext( assets::AssetSystem& assetSystem,
                                                                rhi::GraphicsSystem& graphicsSystem )
    : m_assetSystem( &assetSystem )
    , m_graphicsSystem( &graphicsSystem ) {}

bool VolumeShaderGraphEditorContext::compile() {
    ONYX_ASSERT( m_graphicsSystem != nullptr );

    volume::VolumeShaderGraphGenerator generator;
    bool hasGenerated = m_graph->generateShader( generator );
    return hasGenerated && rhi::ShaderCompiler::ValidateCode( *m_graphicsSystem, m_graph->getShaderCode() );
}

void VolumeShaderGraphEditorContext::onNodeChanged( const Node& newNode ) {
    graphics::ShaderGraphNode& node = getNodeGraph().getNode< graphics::ShaderGraphNode >( newNode.LocalId );
    node.OnNodeChanged( *m_assetSystem );
}
} // namespace onyx::editor
