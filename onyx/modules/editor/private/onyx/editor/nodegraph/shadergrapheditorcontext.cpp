#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/rhi/shader/shadercompiler.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace onyx::editor {

bool ShaderGraphEditorContext::compile() {
    rhi::PBRShaderGenerator generator;
    bool hasGenerated = m_graph->generateShader( generator );
    return hasGenerated &&
           rhi::shader_compiler::validateCode( getEngineSystem< rhi::GraphicsSystem >(), m_graph->getShaderCode() );
}

void ShaderGraphEditorContext::onNodeChanged( const Node& newNode ) {
    graphics::ShaderGraphNode& node = getNodeGraph().getNode< graphics::ShaderGraphNode >( newNode.LocalId );
    node.onNodeChanged( getEngineSystem< assets::AssetSystem >() );
}
} // namespace onyx::editor
