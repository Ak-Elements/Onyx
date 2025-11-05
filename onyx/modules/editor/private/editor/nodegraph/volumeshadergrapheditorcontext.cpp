#include <editor/nodegraph/volumeshadergrapheditorcontext.h>

#include <onyx/graphics/shader/shadercompiler.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>
#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>

namespace Onyx::Editor
{
    VolumeShaderGraphEditorContext::VolumeShaderGraphEditorContext(Assets::AssetSystem& assetSystem, Graphics::GraphicsSystem& graphicsSystem)
        : m_AssetSystem(&assetSystem)
        , m_GraphicsSystem(&graphicsSystem)
    {
    }

    bool VolumeShaderGraphEditorContext::Compile()
    {
        ONYX_ASSERT(m_GraphicsSystem != nullptr);

        Volume::VolumeShaderGraphGenerator generator;
        bool hasGenerated = Graph->GenerateShader(generator);
        return hasGenerated && Graphics::ShaderCompiler::ValidateCode(*m_GraphicsSystem, Graph->GetShaderCode());
    }

    void VolumeShaderGraphEditorContext::OnNodeChanged(const Node& newNode)
    {
        Graphics::ShaderGraphNode& node = GetNodeGraph().GetNode<Graphics::ShaderGraphNode>(newNode.LocalId);
        node.OnNodeChanged(*m_AssetSystem);
    }
}
