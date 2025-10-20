#include <editor/nodegraph/shadergrapheditorcontext.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shader/shadercompiler.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx::Editor
{
    ShaderGraphEditorContext::ShaderGraphEditorContext(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi)
        : m_AssetSystem(&assetSystem)
        , m_GraphicsApi(&graphicsApi)
    {
    }

    bool ShaderGraphEditorContext::Compile()
    {
        ONYX_ASSERT(m_GraphicsApi != nullptr);

        Graphics::PBRShaderGenerator generator;
        bool hasGenerated = Graph->GenerateShader(generator);
        return hasGenerated && Graphics::ShaderCompiler::ValidateCode(*m_GraphicsApi, Graph->GetShaderCode());
    }

    void ShaderGraphEditorContext::OnNodeChanged(const Node& newNode)
    {
        Graphics::ShaderGraphNode& node = GetNodeGraph().GetNode<Graphics::ShaderGraphNode>(newNode.LocalId);
        node.OnNodeChanged(*m_AssetSystem);
    }
}
