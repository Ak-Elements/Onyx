#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/rhi/shader/shadercompiler.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::editor {
ShaderGraphEditorContext::ShaderGraphEditorContext( assets::AssetSystem& assetSystem,
                                                    rhi::GraphicsSystem& graphicsSystem )
    : m_AssetSystem( &assetSystem )
    , m_graphicsSystem( &graphicsSystem ) {}

bool ShaderGraphEditorContext::compile() {
    ONYX_ASSERT( m_graphicsSystem != nullptr );

    rhi::PBRShaderGenerator generator;
    bool hasGenerated = m_graph->GenerateShader( generator );
    return hasGenerated && rhi::ShaderCompiler::ValidateCode( *m_graphicsSystem, m_graph->GetShaderCode() );
}

void ShaderGraphEditorContext::onNodeChanged( const Node& newNode ) {
    graphics::ShaderGraphNode& node = getNodeGraph().getNode< graphics::ShaderGraphNode >( newNode.LocalId );
    node.OnNodeChanged( *m_AssetSystem );
}
} // namespace onyx::editor
