#pragma once

#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace onyx::graphics
{
    struct ShaderNodeMetaData : public node_graph::NodeEditorMetaData
    {
        InplaceFunction<void(const node_graph::Node& node, node_graph::ExecutionContext& executionContext, String& outString)> GenerateShaderCode;
    };

    class ShaderGraphNodeFactory : public node_graph::TypedNodeFactory<ShaderGraphNode, ShaderNodeMetaData>
    {
    public:
        //TODO: Node concept to enforce node
        template <typename T> requires std::is_base_of_v<ShaderGraphNode, T>
        static void Register()
        {
            TypedNodeFactory::Register<T>();
        }
    };
}


