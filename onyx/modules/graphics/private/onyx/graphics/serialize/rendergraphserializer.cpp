#include <onyx/graphics/serialize/rendergraphserializer.h>

#include <onyx/graphics/graphicssystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/nodegraph/nodegraphserializer.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    class UIRenderGraphNode;

    template <>
    struct Serialization<Graphics::RenderGraph>
    {
        static bool Serialize(Serializer& serializer, const Graphics::RenderGraph& renderGraph)
        {
            const NodeGraph::NodeGraph& nodeGraph = renderGraph.GetNodeGraph();
            NodeGraph::Serialize(serializer, nodeGraph);
            return true;
        }

        static bool Deserialize(const Deserializer& deserializer, Graphics::RenderGraph& outRenderGraph)
        {
            Graphics::RenderGraphNodeFactory factory;
            NodeGraph::NodeGraph& outGraph = outRenderGraph.GetNodeGraph();
            NodeGraph::Deserialize(deserializer, outGraph, factory);
            return true;
        }
    };
}

namespace Onyx::Graphics
{
    RenderGraphSerializer::RenderGraphSerializer(Assets::AssetSystem& assetSystem, GraphicsSystem& graphicsSystem)
        : AssetSerializer(assetSystem)
        , m_GraphicsSystem(&graphicsSystem)
    {
    }

    bool RenderGraphSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, Serializer& serializer) const
    {
#if ONYX_IS_EDITOR
        const RenderGraph& renderGraph = asset.As<RenderGraph>();
        return serializer.Write(renderGraph);
#else
        ONYX_UNUSED(asset);
        ONYX_UNUSED(serializer);
        return false;
#endif
    }

    bool RenderGraphSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, const Deserializer& deserializer) const
    {
        ONYX_ASSERT(m_GraphicsSystem != nullptr);

        RenderGraph& renderGraph = asset.As<RenderGraph>();

        RenderGraphNodeFactory factory;
        NodeGraph::NodeGraph& outGraph = renderGraph.GetNodeGraph();
        if (NodeGraph::Deserialize(deserializer, outGraph, factory) == false)
        {
            return false;
        }

        renderGraph.Init(*m_GraphicsSystem);
        return true;
    }
}
