#include <onyx/entity/entityregistry.h>

namespace Onyx::Entity
{
    EntityId EntityRegistry::CreateEntity()
    {
        return m_Registry.create();
    }

    void EntityRegistry::DeleteEntity(EntityId entityId)
    {
        m_Registry.destroy(entityId);
    }

    EntityId EntityRegistry::CopyEntity(EntityId entity)
    {
        EntityId newEntity = CreateEntity();

        // create a copy of an entity component by component
        for (const auto&& componentStorageIt : GetStorage())
        {
            if (auto& componentStorage = componentStorageIt.second; componentStorage.contains(entity))
            {
                componentStorage.push(newEntity, componentStorage.value(entity));
            }
        }

        return newEntity;
    }

    EntityRegistry::EntityRegistryT::iterable EntityRegistry::GetStorage()
    {
        return m_Registry.storage();
    }

    EntityRegistry::EntityRegistryT::const_iterable EntityRegistry::GetStorage() const
    {
        return m_Registry.storage();
    }

    EntityRegistry::EntityRegistryT& EntityRegistry::GetRegistry()
    {
        return m_Registry;
    }

    const EntityRegistry::EntityRegistryT& EntityRegistry::GetRegistry() const
    {
        return m_Registry;
    }

    void EntityRegistry::Clear()
    {
        m_Registry.clear();
    }
}
