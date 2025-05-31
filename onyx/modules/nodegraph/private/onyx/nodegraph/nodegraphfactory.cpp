#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

namespace Onyx::NodeGraph
{
    HashMap<StringId32, UniquePtr<INodeGraphTypeMeta>> NodeGraphTypeRegistry::s_TypeMeta;
    HashMap<PinTypeId, StringId32> NodeGraphTypeRegistry::s_RuntimeToStaticTypeId;
}
