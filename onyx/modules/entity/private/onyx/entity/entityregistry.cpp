#include <onyx/entity/entityregistry.h>

namespace Onyx::Entity
{
    HashMap<StringId32, entt::id_type> EntityRegistry::s_SerializedIdToMetaClassId;
}
