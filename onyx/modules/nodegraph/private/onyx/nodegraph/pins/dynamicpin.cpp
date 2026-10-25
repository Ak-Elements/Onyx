#include <onyx/nodegraph/pins/dynamicpin.h>

#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

namespace onyx::node_graph {
UniquePtr< PinBase > createPin( StringId32 typeId, Guid64 globalId, StringId32 localId ) {
    const INodeGraphTypeMeta& typeMeta = NodeGraphTypeRegistry::getTypeMeta( typeId );
    return typeMeta.createPin( globalId, localId );
}
} // namespace onyx::node_graph
