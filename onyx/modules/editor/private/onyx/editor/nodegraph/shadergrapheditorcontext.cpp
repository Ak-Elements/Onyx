#include <onyx/editor/nodegraph/shadergrapheditorcontext.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/rhi/shader/shadercompiler.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace onyx::editor
{
    ShaderGraphEditorContext::ShaderGraphEditorContext(assets::AssetSystem& assetSystem, rhi::GraphicsSystem& graphicsSystem)
        : m_AssetSystem(&assetSystem)
        , m_graphicsSystem(&graphicsSystem)
    {
    }

    bool ShaderGraphEditorContext::Compile()
    {
        ONYX_ASSERT(m_GraphicsSystem != nullptr);

        rhi::PBRShaderGenerator generator;
        bool hasGenerated = Graph->GenerateShader(generator);
        return hasGenerated && rhi::ShaderCompiler::ValidateCode(*m_graphicsSystem, Graph->GetShaderCode());
    }

    void ShaderGraphEditorContext::OnNodeChanged(const Node& newNode)
    {
        graphics::ShaderGraphNode& node = GetNodeGraph().GetNode<graphics::ShaderGraphNode>(newNode.LocalId);
        node.OnNodeChanged(*m_AssetSystem);
    }
}
