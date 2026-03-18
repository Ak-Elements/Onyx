#include <onyx/graphics/serialize/shadergraphserializer.h>

#include <onyx/graphics/shadergraph/shadergraph.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace onyx
{
    template <>
    struct Serialization<graphics::ShaderGraph>
    {
        static bool Serialize(Serializer& serializer, const graphics::ShaderGraph& nodeGraphNode)
        {
            return nodeGraphNode.Serialize(serializer);
        }

        static bool Deserialize(const Deserializer& deserializer, graphics::ShaderGraph& outGraphNode)
        {
            return outGraphNode.Deserialize(deserializer);
        }
    };
}

namespace onyx::graphics::shader_graph_serializer
{
    bool Serialize(const ShaderGraph& graph, Serializer& serializer)
    {
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        return serializer.Write(graph);
#else
        ONYX_UNUSED(graph);
        ONYX_UNUSED(serializer);
        return true;
#endif

    }

    bool Deserialize(ShaderGraph& graph, const Deserializer& deserializer)
    {
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        return deserializer.Read(graph);
#else
        ONYX_UNUSED(graph);
        ONYX_UNUSED(deserializer);
        return true;
#endif
    }
}
