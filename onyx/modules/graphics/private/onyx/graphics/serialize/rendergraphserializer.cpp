#include <onyx/graphics/serialize/rendergraphserializer.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/nodegraph/nodegraphserializer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
class UIRenderGraphNode;

template <>
struct Serialization< graphics::RenderGraph > {
    static bool serialize( Serializer& serializer, const graphics::RenderGraph& renderGraph ) {
        const node_graph::NodeGraph& nodeGraph = renderGraph.GetNodeGraph();
        node_graph::serialize( serializer, nodeGraph );
        return true;
    }

    static bool deserialize( const Deserializer& deserializer, graphics::RenderGraph& outRenderGraph ) {
        graphics::RenderGraphNodeFactory factory;
        node_graph::NodeGraph& outGraph = outRenderGraph.GetNodeGraph();
        node_graph::deserialize( deserializer, outGraph, factory );
        return true;
    }
};
} // namespace onyx

namespace onyx::graphics {
bool RenderGraphSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                       const assets::AssetMetaData& /*meta*/,
                                       Serializer& serializer,
                                       const IEngine& /*engine*/ ) const {
#if ONYX_IS_EDITOR
    const RenderGraph& renderGraph = asset.as< RenderGraph >();
    return serializer.write( renderGraph );
#else
    ONYX_UNUSED( asset );
    ONYX_UNUSED( serializer );
    return false;
#endif
}

bool RenderGraphSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                         const assets::AssetMetaData& /*meta*/,
                                         const Deserializer& deserializer,
                                         IEngine& /*engine*/ ) const {
    RenderGraph& renderGraph = asset.as< RenderGraph >();

    RenderGraphNodeFactory factory;
    node_graph::NodeGraph& outGraph = renderGraph.GetNodeGraph();
    if ( node_graph::deserialize( deserializer, outGraph, factory ) == false ) {
        return false;
    }

    return true;
}
} // namespace onyx::graphics
