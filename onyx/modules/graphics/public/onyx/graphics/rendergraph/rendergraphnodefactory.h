#pragma once

#include <onyx/nodegraph/nodegraphfactory.h>

#include <onyx/graphics/rendergraph/rendergraphtask.h>

namespace Onyx::Graphics
{
    struct RenderGraphNodeEditorMetaData : NodeGraph::NodeEditorMetaData
    {
    };

    class RenderGraphNodeFactory : public NodeGraph::TypedNodeFactory<IRenderGraphNode, RenderGraphNodeEditorMetaData>
    {
    public:
        //TODO: Node concept to enforce node
        template <typename T> /*requires std::derived_from<Graphics::RenderGraphTask, T>*/
        static void RegisterNode(const StringView& nodeName)
        {
            TypedNodeFactory::RegisterNode<T>(nodeName);
        }
    };
}