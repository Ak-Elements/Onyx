#pragma once

#include <onyx/nodegraph/nodegraphfactory.h>

#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace onyx::graphics
{
    struct RenderGraphNodeEditorMetaData : node_graph::NodeEditorMetaData
    {
    };

    class RenderGraphNodeFactory : public node_graph::TypedNodeFactory<IRenderGraphNode, RenderGraphNodeEditorMetaData>
    {
    public:
        //TODO: Node concept to enforce node
        template <typename T> /*requires std::derived_from<graphics::RenderGraphTask, T>*/
        static void Register()
        {
            TypedNodeFactory::Register<T>();
        }
    };
}