#include <onyx/editor/nodegraph/volumeshadergrapheditorcontext.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/rhi/shader/shadercompiler.h>
#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>
#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>

namespace onyx::editor {

bool VolumeShaderGraphEditorContext::compile() {
    volume::VolumeShaderGraphGenerator generator;
    bool hasGenerated = m_graph->generateShader( generator );
    return hasGenerated &&
           rhi::shader_compiler::validateCode( getEngineSystem< rhi::GraphicsSystem >(), m_graph->getShaderCode() );
}

void VolumeShaderGraphEditorContext::onNodeChanged( const Node& newNode ) {
    graphics::ShaderGraphNode& node = getNodeGraph().getNode< graphics::ShaderGraphNode >( newNode.LocalId );
    node.onNodeChanged( getEngineSystem< assets::AssetSystem >() );
}
} // namespace onyx::editor
