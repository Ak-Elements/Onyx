#pragma once

#include <onyx/nodegraph/nodefactory.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::NodeGraph
{
    struct ShaderNodeMetaData : public NodeEditorMetaData
    {
        InplaceFunction<void(const Node& node, ExecutionContext& executionContext, String& outString)> GenerateShaderCode;
    };

    class ShaderNodeFactory : public TypedNodeFactory<Graphics::ShaderGraphNode, ShaderNodeMetaData>
    {
    public:
        //TODO: Node concept to enforce node
        template <typename T> requires std::is_base_of_v<Graphics::ShaderGraphNode, T>
        static void RegisterNode(const StringView& nodeName)
        {
            TypedNodeFactory::RegisterNode<T>(nodeName);
        }
    };

}
