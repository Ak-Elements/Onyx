#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/rhi/shader/shadercompiler.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::editor {
ShaderGraphEditorContext::ShaderGraphEditorContext( assets::AssetSystem& assetSystem,
                                                    rhi::GraphicsSystem& graphicsSystem )
    : m_assetSystem( &assetSystem )
    , m_graphicsSystem( &graphicsSystem ) {
    m_graph = assetSystem.create< graphics::MaterialShaderGraph >();
}

bool ShaderGraphEditorContext::compile() {
    ONYX_ASSERT( m_graphicsSystem != nullptr );

    rhi::PBRShaderGenerator generator;
    bool hasGenerated = m_graph->generateShader( generator );
    return hasGenerated && rhi::shader_compiler::validateCode( *m_graphicsSystem, m_graph->getShaderCode() );
}

void ShaderGraphEditorContext::onNodeChanged( const Node& newNode ) {
    graphics::ShaderGraphNode& node = getNodeGraph().getNode< graphics::ShaderGraphNode >( newNode.LocalId );
    node.onNodeChanged( *m_assetSystem );
}
} // namespace onyx::editor
