#include <onyx/nodegraph/pin.h>

#include <onyx/nodegraph/nodegraphfactory.h>

namespace Onyx::NodeGraph
{
    UniquePtr<PinBase> CreatePin(PinTypeId typeId, Guid64 globalId, onyxU32 localId, const String& localIdString)
    {
        return NodeGraphTypeRegistry::CreatePin(typeId, globalId, localId, localIdString);
    }

}
