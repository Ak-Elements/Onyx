#include <onyx/input/inputbindingsregistry.h>

namespace Onyx::Input
{
    HashMap<ActionType, HashSet<StringId32>>  InputBindingsRegistry::ms_RegisteredBindingsPerActionType;
    HashMap<StringId32, InplaceFunction<UniquePtr<InputBinding>()>> InputBindingsRegistry::ms_RegisteredBindings;
    HashMap<StringId32, String> InputBindingsRegistry::ms_RegisteredBindingNames;

}
