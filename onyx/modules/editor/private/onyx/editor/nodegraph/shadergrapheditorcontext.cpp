#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>

#include <onyx/graphics/graphicssystem.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shader/shadercompiler.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx::Editor
{
    ShaderGraphEditorContext::ShaderGraphEditorContext(Assets::AssetSystem& assetSystem, Graphics::GraphicsSystem& graphicsSystem)
        : m_AssetSystem(&assetSystem)
        , m_GraphicsSystem(&graphicsSystem)
    {
    }

    bool ShaderGraphEditorContext::Compile()
    {
        ONYX_ASSERT(m_GraphicsSystem != nullptr);

        Graphics::PBRShaderGenerator generator;
        bool hasGenerated = Graph->GenerateShader(generator);
        return hasGenerated && Graphics::ShaderCompiler::ValidateCode(*m_GraphicsSystem, Graph->GetShaderCode());
    }

    void ShaderGraphEditorContext::OnNodeChanged(const Node& newNode)
    {
        Graphics::ShaderGraphNode& node = GetNodeGraph().GetNode<Graphics::ShaderGraphNode>(newNode.LocalId);
        node.OnNodeChanged(*m_AssetSystem);
    }
}
