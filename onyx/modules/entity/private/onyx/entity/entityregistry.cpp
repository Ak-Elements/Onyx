#include <onyx/entity/entityregistry.h>

#include <onyx/entity/componentfactory.h>
#include <onyx/entity/ecsexecutioncontext.h>

namespace Onyx::Entity
{
    EntityRegistry::EntityRegistry(ComponentFactory& factory)
        : m_ComponentFactory(&factory)
    {
    }
    
    EntityRegistry::EntityRegistry(const EntityRegistry& other)
        : m_ComponentFactory(other.m_ComponentFactory)
    {
        m_Registry.clear();
        other.Copy(*this);
    }

    EntityRegistry::EntityRegistry(EntityRegistry&& other)
        : m_Registry(std::move(other.m_Registry))
        , m_ComponentFactory(other.m_ComponentFactory)
    {
    }

    EntityRegistry& EntityRegistry::operator=(const EntityRegistry& other)
    {
        if (this == &other)
            return *this;

        m_Registry.clear();
        m_ComponentFactory = other.m_ComponentFactory;
        other.Copy(*this);

        return *this;
    }

    EntityRegistry& EntityRegistry::operator=(EntityRegistry&& other)
    {
        if (this == &other)
            return *this;

        other.m_Registry.swap(m_Registry);
        m_ComponentFactory = other.m_ComponentFactory;

        return *this;
    }

    EntityId EntityRegistry::CreateEntity()
    {
        return m_Registry.create();
    }

    EntityId EntityRegistry::CreateEntity(EntityId entity)
    {
        ONYX_ASSERT(entity != EntityId::Invalid);
        return m_Registry.create(entity);
    }

    void EntityRegistry::DeleteEntity(EntityId entity)
    {
        ONYX_ASSERT(entity != EntityId::Invalid);
        m_Registry.destroy(entity);
    }

    EntityId EntityRegistry::CopyEntity(EntityId entity)
    {
        ONYX_ASSERT(entity != EntityId::Invalid);
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

    EntityRegistry::EntityRegistryT::common_type* EntityRegistry::GetStorage(entt::id_type runtimeTypeId)
    {
        return m_Registry.storage(runtimeTypeId);
    }

    EntityRegistry::EntityRegistryT& EntityRegistry::GetRegistry()
    {
        return m_Registry;
    }

    const EntityRegistry::EntityRegistryT& EntityRegistry::GetRegistry() const
    {
        return m_Registry;
    }

    void EntityRegistry::Copy(EntityRegistry& toRegistry) const
    {
        ONYX_ASSERT(m_ComponentFactory != nullptr);

        //ONYX_ASSERT(toRegistry.IsEmpty(), "Registry to copy to has to be empty.");
        // create a copy of an entity component by component
        for (const auto&& componentStorageIt : GetStorage())
        {
            const IComponentMeta* componentMeta = m_ComponentFactory->GetComponentMeta(componentStorageIt.first).value_or(nullptr);
            if (componentMeta == nullptr)
                continue;

            for (EntityId entityId : componentStorageIt.second)
            {
                if ( toRegistry.HasEntity( entityId ) == false)
                {
                    [[maybe_unused]] EntityId newEntityId = toRegistry.CreateEntity( entityId );
                    ONYX_ASSERT(newEntityId == entityId);
                }
                
                const void* fromComponentPtr = componentStorageIt.second.value(entityId);
                componentMeta->Copy(toRegistry, entityId, fromComponentPtr);
            }
        }
    }

    void EntityRegistry::Clear()
    {
        m_Registry.clear();
    }

    const EntityRegistry& DependantFunctionArg<EntityRegistry>::Get(const ECSExecutionContext& context)
    {
        return context.Registry;
    }
}
