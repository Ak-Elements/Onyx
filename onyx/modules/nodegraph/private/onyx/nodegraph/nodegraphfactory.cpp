#include <onyx/nodegraph/nodegraphfactory.h>

namespace Onyx::NodeGraph
{
    HashMap<StringId32, InplaceFunction<PinBase* (Guid64, StringId32)>> NodeGraphTypeRegistry::s_RegisteredTypes;
}