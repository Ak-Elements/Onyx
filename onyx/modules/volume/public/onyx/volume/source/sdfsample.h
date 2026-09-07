#pragma once

#include <onyx/filesystem/onyxfile.h>
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
    static void Register() { NodeGraphTypeRegistry::Register< onyx::volume::SdfSample, "onyx::volume::SdfSample" >(); }

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( [[maybe_unused]] StringView name, [[maybe_unused]] volume::SdfSample& value ) {
        return true;
    }
    static constexpr uint32_t GetPinTypeColor() { return 0xFFD79633; };

    // TODO: Remove
    static bool serialize( [[maybe_unused]] file_system::JsonValue& json,
                           [[maybe_unused]] const onyx::volume::SdfSample& handle ) {
        return true;
    }

    static bool deserialize( [[maybe_unused]] const file_system::JsonValue& json,
                             [[maybe_unused]] onyx::volume::SdfSample& handle ) {
        return true;
    }
#endif
};

} // namespace onyx::node_graph
