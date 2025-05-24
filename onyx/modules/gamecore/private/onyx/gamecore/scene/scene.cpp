#include <onyx/gamecore/scene/scene.h>

#include <onyx/entity/entity.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/gamecore/components/freecameracomponent.h>
#include <onyx/gamecore/components/transientcomponent.h>

namespace Onyx::GameCore
{
    Scene::Scene()
    {
        m_Registry.GetRegistry().on_construct<TransformComponent>().connect<&Scene::OnTransformComponentConstructed>(this);
        m_Registry.GetRegistry().on_destroy<TransformComponent>().connect<&Scene::OnTransformComponentDestroyed>(this);
    }

    void Scene::SetLoadCenter(const Vector3f& loadCenter)
    {
        if (loadCenter != m_LoadCenter)
        {
            m_LoadCenter = loadCenter;
        }
    }

    void Scene::SetStreamInDistance(onyxF64 distance)
    {
        m_SectorStreamer.SetStreamInDistance(distance);
    }

    void Scene::SetStreamOutDistance(onyxF64 distance)
    {
        m_SectorStreamer.SetStreamOutDistance(distance);
    }

    void Scene::Copy(const Reference<Scene>& fromScene)
    {
        m_Registry.Clear();

        // all entities
        const Entity::EntityRegistry& fromRegistry = fromScene->GetRegistry();
        for (Entity::EntityId sourceEntityId : fromRegistry.GetView<Entity::EntityId>())
        {
            Entity::EntityId targetEntity = m_Registry.CreateEntity();
            // create a copy of an entity component by component
            for (auto&& componentStorageIt : fromRegistry.GetStorage())
            {
                if (auto& componentStorage = componentStorageIt.second; componentStorage.contains(sourceEntityId))
                {
                    entt::meta_type metaClass = entt::resolve(componentStorageIt.first);
                    
                    if (!metaClass)
                    {
                        ONYX_LOG_ERROR("Failed copying component of entity during scene copy.");
                        continue;
                    }

                    const entt::meta_any componentHandle = metaClass.from_void(componentStorage.value(sourceEntityId));
                    
                    std::ignore = metaClass.construct(entt::forward_as_meta(m_Registry.GetRegistry()), targetEntity, componentHandle);
                }
            }
        }
    }

    void Scene::Update(onyxU64 /*deltaTime*/)
    {
        m_SectorStreamer.Update(m_LoadCenter);
    }

    void Scene::OnTransformComponentConstructed(Entity::EntityRegistry::EntityRegistryT& /*registry*/, Entity::EntityId entity)
    {
        //if (m_Registry.HasComponents<TransientComponent>(entity))
        //{
        //    return;
        //}

        m_SectorStreamer.AddEntity(entity);
    }

    void Scene::OnTransformComponentDestroyed(Entity::EntityRegistry::EntityRegistryT& /*registry*/, Entity::EntityId entity)
    {
       // if (m_Registry.HasComponents<TransientComponent>(entity))
        //{
       //     return;
       // }

        m_SectorStreamer.RemoveEntity(entity);
    }
}
