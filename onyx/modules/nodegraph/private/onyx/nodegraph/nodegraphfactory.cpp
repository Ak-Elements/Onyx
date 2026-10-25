#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

namespace onyx::node_graph {
HashMap< StringId32, UniquePtr< INodeGraphTypeMeta > > NodeGraphTypeRegistry::s_typeMeta;
HashMap< PinTypeId, StringId32 > NodeGraphTypeRegistry::s_runtimeToStaticTypeId;
} // namespace onyx::node_graph
