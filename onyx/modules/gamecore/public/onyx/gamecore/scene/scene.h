#pragma once
#include <onyx/assets/asset.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/scene/scenesectorstreamer.h>

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/physics/physicssystem.h>

namespace onyx::graphics {
class RenderGraph;
}

namespace onyx::ecs {
class ComponentFactory;
}

namespace onyx::game_core {
class Scene : public assets::Asset< Scene > {
    friend struct SceneSerializer;

  public:
    static constexpr StringId32 TypeId{ "onyx::game_core::assets::Scene" };
    static StringId32 getTypeId() { return TypeId; }

    static Reference< Scene > create( IEngine& engine );

    Scene( ecs::ComponentFactory& factory );

    ecs::EntityRegistry& getRegistry() { return m_registry; }
    const ecs::EntityRegistry& getRegistry() const { return m_registry; }

    const SceneSectorStreamer& getSectorStreamer() const { return m_sectorStreamer; }

    void setLoadCenter( const Vector3f32& loadCenter );
    void setStreamInDistance( float64 distance );
    void setStreamOutDistance( float64 distance );

    void copy( const Reference< Scene >& scene );

    void update( uint64_t deltaTime );

    bool hasRenderGraph() { return m_sceneRenderGraph.isValid() && m_sceneRenderGraph->isInitialized(); }

    assets::AssetHandle< graphics::RenderGraph >& getRenderGraphRef() { return m_sceneRenderGraph; }
    graphics::RenderGraph& getRenderGraph() { return *m_sceneRenderGraph; }
    const graphics::RenderGraph& getRenderGraph() const { return *m_sceneRenderGraph; }

    onyx::physics::PhysicsWorld& getPhysicsWorld() { return m_physicsWorld; }

#if ONYX_IS_EDITOR
    String getUniqueEntityName( const String& preferredName );
#endif

  private:
    // only needed in editor most likely
    void onTransformComponentConstructed( ecs::EntityRegistry::EntityRegistryT& registry, ecs::EntityId entity );
    void onTransformComponentDestroyed( ecs::EntityRegistry::EntityRegistryT& registry, ecs::EntityId entity );

  private:
    Vector3f32 m_loadCenter; // center position of the streaming

    SceneSectorStreamer m_sectorStreamer{ *this };
    ecs::EntityRegistry m_registry;
    onyx::physics::PhysicsWorld m_physicsWorld;

    assets::AssetHandle< graphics::RenderGraph > m_sceneRenderGraph;
};
} // namespace onyx::game_core
