#pragma once
#include <onyx/assets/asset.h>
#include <onyx/gamecore/scene/scenesectorstreamer.h>
#include <onyx/entity/entityregistry.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/physics/physicssystem.h>

namespace onyx::graphics
{
    class RenderGraph;
}

namespace onyx::ecs
{
    class ComponentFactory;
}

namespace onyx::game_core
{
    class Scene : public assets::Asset<Scene>
    {
        friend struct SceneSerializer;
    public:
        static constexpr StringId32 TypeId{ "onyx::game_core::assets::Scene" };
        
        static Reference<Scene> Create(IEngine& engine);

        Scene(ecs::ComponentFactory& factory);

        StringId32 GetTypeId() const { return TypeId; }

        ecs::EntityRegistry& GetRegistry() { return m_Registry; }
        const ecs::EntityRegistry& GetRegistry() const { return m_Registry; }

        const SceneSectorStreamer& GetSectorStreamer() const { return m_SectorStreamer; }

        void SetLoadCenter(const Vector3f32& loadCenter);
        void SetStreamInDistance(onyxF64 distance);
        void SetStreamOutDistance(onyxF64 distance);

        void Copy(const Reference<Scene>& scene);

        void Update(onyxU64 deltaTime);

        bool HasRenderGraph() { return m_SceneRenderGraph.IsValid() && m_SceneRenderGraph->IsInitialized(); }

        assets::AssetHandle<graphics::RenderGraph>& GetRenderGraphRef() { return m_SceneRenderGraph; }
        graphics::RenderGraph& GetRenderGraph() { return *m_SceneRenderGraph; }
        const graphics::RenderGraph& GetRenderGraph() const { return *m_SceneRenderGraph; }

        onyx::physics::PhysicsWorld& GetPhysicsWorld() { return m_PhysicsWorld; }

#if ONYX_IS_EDITOR
        String GetUniqueEntityName(const String& preferredName);
#endif

    private:
        // only needed in editor most likely 
        void OnTransformComponentConstructed(ecs::EntityRegistry::EntityRegistryT& registry, ecs::EntityId entity);
        void OnTransformComponentDestroyed(ecs::EntityRegistry::EntityRegistryT& registry, ecs::EntityId entity);

    private:
        Vector3f32 m_LoadCenter; // center position of the streaming

        SceneSectorStreamer m_SectorStreamer { *this };
        ecs::EntityRegistry m_Registry;
        onyx::physics::PhysicsWorld m_PhysicsWorld;

        assets::AssetHandle<graphics::RenderGraph> m_SceneRenderGraph;
    };
}
