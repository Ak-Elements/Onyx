#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

namespace onyx::node_graph
{
    HashMap<StringId32, UniquePtr<INodeGraphTypeMeta>> NodeGraphTypeRegistry::s_TypeMeta;
    HashMap<PinTypeId, StringId32> NodeGraphTypeRegistry::s_RuntimeToStaticTypeId;
}
