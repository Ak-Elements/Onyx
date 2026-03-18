#include <onyx/graphics/serialize/rendergraphserializer.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/nodegraph/nodegraphserializer.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace onyx
{
    class UIRenderGraphNode;

    template <>
    struct Serialization<graphics::RenderGraph>
    {
        static bool Serialize(Serializer& serializer, const graphics::RenderGraph& renderGraph)
        {
            const node_graph::NodeGraph& nodeGraph = renderGraph.GetNodeGraph();
            node_graph::Serialize(serializer, nodeGraph);
            return true;
        }

        static bool Deserialize(const Deserializer& deserializer, graphics::RenderGraph& outRenderGraph)
        {
            graphics::RenderGraphNodeFactory factory;
            node_graph::NodeGraph& outGraph = outRenderGraph.GetNodeGraph();
            node_graph::Deserialize(deserializer, outGraph, factory);
            return true;
        }
    };
}

namespace onyx::graphics
{
    bool RenderGraphSerializer::Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& /*meta*/, Serializer& serializer, const IEngine& /*engine*/) const
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

    bool RenderGraphSerializer::Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& /*meta*/, const Deserializer& deserializer, IEngine& /*engine*/) const
    {
        RenderGraph& renderGraph = asset.As<RenderGraph>();

        RenderGraphNodeFactory factory;
        node_graph::NodeGraph& outGraph = renderGraph.GetNodeGraph();
        if (node_graph::Deserialize(deserializer, outGraph, factory) == false)
        {
            return false;
        }

        return true;
    }
}
