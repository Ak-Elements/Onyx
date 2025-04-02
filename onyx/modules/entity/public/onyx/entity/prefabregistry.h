#pragma once

#include <onyx/entity/entityregistry.h>
#include <onyx/onyx_types.h>

namespace Onyx::Entity
{
    struct Prefab
    {
        EntityId GetId() { return m_Entity.GetId(); }
        EntityId GetId() const { return m_Entity.GetId(); }

        Entity m_Entity;
    };

    class PrefabRegistry
    {
    public:
        Prefab CreateEntity()
        {
            return { m_PrefabRegistry.CreateEntity() } ;
        }

        void AddComponent(EntityId entityId, onyxU32 componentId, const FileSystem::JsonValue& json)
        {
            m_PrefabRegistry.AddComponent(entityId, componentId, json);
        }

        template <typename T, typename... Args>
        T& AddComponent(Prefab prefab, Args&&... args)
        {
            return m_PrefabRegistry.AddComponent(prefab.m_Entity.GetId(), std::forward<Args>(args)...);
        }

        template <typename T>
        void RemoveComponent(Prefab prefab)
        {
            m_PrefabRegistry.RemoveComponent<T>(prefab.m_Entity.GetId());
        }

        void RegisterPrefab(onyxU32 id, Prefab entity) { m_Prefabs[id] = entity; }
        EntityRegistry& GetRegistry() { return m_PrefabRegistry; }

        Entity Instantiate(onyxU32 prefabId, EntityRegistry& registry)
        {
#if ONYX_IS_DEBUG
            if (m_Prefabs.contains(prefabId) == false)
            {
                ONYX_LOG_ERROR("Prefab with id {} is unkown.", prefabId);
                return {};
            }
#endif

            return Instantiate(m_Prefabs[prefabId], registry);
        }

        Entity Instantiate(Prefab prefab, EntityRegistry& registry)
        {
            Entity newEntity = registry.CreateEntity();

            EntityId prefabEntityId = prefab.m_Entity.GetId();
            // create a copy of an entity component by component
            for (const auto&& componentStorageIt : m_PrefabRegistry.GetRegistry().storage())
            {
                if (const auto& prefabComponentStorage = componentStorageIt.second; prefabComponentStorage.contains(prefabEntityId))
                {
                    const auto sceneComponentStorage = registry.GetRegistry().storage(componentStorageIt.first);
                    if (sceneComponentStorage == nullptr)
                    {
                        ONYX_LOG_ERROR("Unknown component id.");
                        continue;
                    }

                    sceneComponentStorage->push(newEntity.GetId(), prefabComponentStorage.value(prefabEntityId));
                }
            }

            return newEntity;
        }

    private:
        EntityRegistry m_PrefabRegistry;
        HashMap<onyxU32, Prefab> m_Prefabs;
    };
}
