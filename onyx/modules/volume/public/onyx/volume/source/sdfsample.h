#pragma once

#include <onyx/filesystem/file.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>
namespace onyx::volume {
struct SdfSample {
    Vector3f32 Gradient;
    float Distance = 0.0f;
};
} // namespace onyx::volume

namespace onyx {
template <>
struct Serialization< onyx::volume::SdfSample > {
    static bool serialize( Serializer&, const onyx::volume::SdfSample& ) {
        // not supported but needed for the graph pins
        return true;
    }

    static bool deserialize( const Deserializer&, onyx::volume::SdfSample& ) {
        // not supported but needed for the graph pins
        return true;
    }
};
} // namespace onyx

namespace onyx::node_graph {
template < typename T >
struct PinMetaObject;

template <>
struct PinMetaObject< onyx::volume::SdfSample > {
    static void registerType() {
        NodeGraphTypeRegistry::registerType< onyx::volume::SdfSample, "onyx::volume::SdfSample" >();
    }

#if ONYX_IS_EDITOR
    static bool drawPinInPropertyGrid( [[maybe_unused]] StringView name, [[maybe_unused]] volume::SdfSample& value ) {
        return true;
    }
    static constexpr uint32_t getPinTypeColor() { return 0xFFD79633; };
#endif
};

} // namespace onyx::node_graph
