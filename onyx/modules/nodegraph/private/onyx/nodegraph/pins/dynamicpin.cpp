#include <onyx/nodegraph/pins/dynamicpin.h>

#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

namespace Onyx::NodeGraph
{
    UniquePtr<PinBase> CreatePin(StringId32 typeId, Guid64 globalId, StringId32 localId)
    {
        const INodeGraphTypeMeta& typeMeta = NodeGraphTypeRegistry::GetTypeMeta(typeId);
        return typeMeta.CreatePin(globalId, localId);
    }
}
