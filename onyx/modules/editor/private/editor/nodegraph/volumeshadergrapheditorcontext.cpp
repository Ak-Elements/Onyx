#include <editor/nodegraph/volumeshadergrapheditorcontext.h>

#include <onyx/graphics/shader/shadercompiler.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>
#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>

namespace Onyx::Editor
{
    VolumeShaderGraphEditorContext::VolumeShaderGraphEditorContext(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi)
        : m_AssetSystem(&assetSystem)
        , m_GraphicsApi(&graphicsApi)
    {
    }

    bool VolumeShaderGraphEditorContext::Compile()
    {
        ONYX_ASSERT(m_GraphicsApi != nullptr);

        Volume::VolumeShaderGraphGenerator generator;
        bool hasGenerated = Graph->GenerateShader(generator);
        return hasGenerated && Graphics::ShaderCompiler::ValidateCode(*m_GraphicsApi, Graph->GetShaderCode());
    }

    void VolumeShaderGraphEditorContext::OnNodeChanged(const Node& newNode)
    {
        Graphics::ShaderGraphNode& node = GetNodeGraph().GetNode<Graphics::ShaderGraphNode>(newNode.LocalId);
        node.OnNodeChanged(*m_AssetSystem);
    }
}
