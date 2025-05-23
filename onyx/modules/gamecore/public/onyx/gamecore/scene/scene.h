#pragma once
#include <onyx/assets/asset.h>
#include <onyx/gamecore/scene/scenesectorstreamer.h>
#include <onyx/entity/entityregistry.h>

namespace Onyx::GameCore
{
    class Scene : public Assets::Asset<Scene>
    {
        friend struct SceneSerializer;
    public:
        static constexpr StringId32 TypeId{ "Onyx::GameCore::Assets::Scene" };
        
        Scene();

        StringId32 GetTypeId() const { return TypeId; }

        Entity::EntityRegistry& GetRegistry() { return m_Registry; }
        const Entity::EntityRegistry& GetRegistry() const { return m_Registry; }

        const SceneSectorStreamer& GetSectorStreamer() const { return m_SectorStreamer; }

        void SetLoadCenter(const Vector3f& loadCenter);
        void SetStreamInDistance(onyxF64 distance);
        void SetStreamOutDistance(onyxF64 distance);

        void Copy(const Reference<Scene>& scene);

        void Update(onyxU64 deltaTime);

    private:
        // only needed in editor most likely 
        void OnTransformComponentConstructed(Entity::EntityRegistry::EntityRegistryT& registry, Entity::EntityId entity);
        void OnTransformComponentDestroyed(Entity::EntityRegistry::EntityRegistryT& registry, Entity::EntityId entity);

    private:
        Vector3f m_LoadCenter; // center position of the streaming

        SceneSectorStreamer m_SectorStreamer { *this };
        Entity::EntityRegistry m_Registry;
    };
}
