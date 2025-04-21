#include <onyx/nodegraph/nodegraphfactory.h>

namespace Onyx::NodeGraph
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    HashMap<PinTypeId, InplaceFunction<PinBase* (Guid64, onyxU32, const String&)>> NodeGraphTypeRegistry::s_RegisteredTypes;
#else
    HashMap<PinTypeId, InplaceFunction<PinBase* (Guid64, onyxU32)>> NodeGraphTypeRegistry::s_RegisteredTypes;
#endif
}