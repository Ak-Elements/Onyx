#include <onyx/entity/system.h>

namespace Onyx::Entity
{
    HashMap<StringId32, entt::id_type> EntityRegistry::s_SerializedIdToMetaClassId;
}
