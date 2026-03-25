#include <onyx/graphics/serialize/shadergraphserializer.h>

#include <onyx/graphics/shadergraph/shadergraph.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
template <>
struct Serialization< graphics::ShaderGraph > {
    static bool serialize( Serializer& serializer, const graphics::ShaderGraph& nodeGraphNode ) {
        return nodeGraphNode.serialize( serializer );
    }

    static bool deserialize( const Deserializer& deserializer, graphics::ShaderGraph& outGraphNode ) {
        return outGraphNode.deserialize( deserializer );
    }
};
} // namespace onyx

namespace onyx::graphics::shader_graph_serializer {
bool serialize( const ShaderGraph& graph, Serializer& serializer ) {
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
    return serializer.write( graph );
#else
    ONYX_UNUSED( graph );
    ONYX_UNUSED( serializer );
    return true;
#endif
}

bool deserialize( ShaderGraph& graph, const Deserializer& deserializer ) {
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
    return deserializer.read( graph );
#else
    ONYX_UNUSED( graph );
    ONYX_UNUSED( deserializer );
    return true;
#endif
}
} // namespace onyx::graphics::shader_graph_serializer
