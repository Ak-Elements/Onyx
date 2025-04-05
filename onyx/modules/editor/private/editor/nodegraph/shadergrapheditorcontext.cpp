#include <editor/nodegraph/shadergrapheditorcontext.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx::Editor
{
    ShaderGraphEditorContext::ShaderGraphEditorContext(Assets::AssetSystem& assetSystem)
        : m_AssetSystem(&assetSystem)
    {
    }

    bool ShaderGraphEditorContext::Compile()
    {
        Graphics::PBRShaderGenerator generator;
        return Graph->GenerateShader(generator);
    }

    void ShaderGraphEditorContext::OnNodeChanged(const Node& newNode)
    {
        Graphics::ShaderGraphNode& node = GetNodeGraph().GetNode<Graphics::ShaderGraphNode>(newNode.LocalId);
        node.OnNodeChanged(*m_AssetSystem);
    }
}
