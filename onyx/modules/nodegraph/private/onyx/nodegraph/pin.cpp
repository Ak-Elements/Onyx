#include <onyx/nodegraph/pin.h>

#include <onyx/nodegraph/nodegraphfactory.h>

namespace Onyx::NodeGraph
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    UniquePtr<PinBase> CreatePin(StringId32 typeId, Guid64 globalId, onyxU32 localId, const String& localIdString)
    {
        return NodeGraphTypeRegistry::CreatePin(typeId, globalId, localId, localIdString);
    }
#else
    UniquePtr<PinBase> CreatePin(StringId32 typeId, Guid64 globalId, onyxU32 localId)
    {
        return NodeGraphTypeRegistry::CreatePin(typeId, globalId, localId);
    }
#endif
}
