#include <onyx/entity/entityregistry.h>

namespace Onyx::Entity
{
    HashMap<entt::id_type, StringId32> EntityRegistry::s_RuntimeTypeIdToStaticTypeId;
    HashMap<StringId32, InplaceFunction<void(EntityRegistry&, EntityId, std::any&)>> EntityRegistry::s_Factories;
    ComponentRegistry EntityRegistry::s_ComponentRegistry;
}
