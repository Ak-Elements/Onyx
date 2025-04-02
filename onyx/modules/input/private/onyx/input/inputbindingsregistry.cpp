#include <onyx/input/inputbindingsregistry.h>

namespace Onyx::Input
{
    HashMap<ActionType, HashSet<onyxU32>>  InputBindingsRegistry::ms_RegisteredBindingsPerActionType;
    HashMap<onyxU32, InplaceFunction<UniquePtr<InputBinding>()>> InputBindingsRegistry::ms_RegisteredBindings;
    HashMap<onyxU32, String> InputBindingsRegistry::ms_RegisteredBindingNames;

}
