#pragma once

#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Graphics
{
    struct ShaderNodeMetaData : public NodeGraph::NodeEditorMetaData
    {
        InplaceFunction<void(const NodeGraph::Node& node, NodeGraph::ExecutionContext& executionContext, String& outString)> GenerateShaderCode;
    };

    class ShaderGraphNodeFactory : public NodeGraph::TypedNodeFactory<ShaderGraphNode, ShaderNodeMetaData>
    {
    public:
        //TODO: Node concept to enforce node
        template <typename T> requires std::is_base_of_v<ShaderGraphNode, T>
        static void RegisterNode(const StringView& nodeName)
        {
            TypedNodeFactory::RegisterNode<T>(nodeName);
        }
    };
}
