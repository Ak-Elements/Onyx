#include <onyx/nodegraph/pins/dynamicpin.h>

#include <onyx/nodegraph/nodegraphfactory.h>

namespace Onyx::NodeGraph
{
    UniquePtr<PinBase> CreatePin(StringId32 typeId, Guid64 globalId, StringId32 localId)
    {
        return NodeGraphTypeRegistry::CreatePin(typeId, globalId, localId);
    }
}
