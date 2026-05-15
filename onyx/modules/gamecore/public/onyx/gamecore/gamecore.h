#pragma once
#include <onyx/engine/enginesystem.h>
#include <onyx/entity/componentfactory.h>
#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/debug/debugdrawqueue.h>

namespace onyx::rhi {
class GraphicsSystem;
}

namespace onyx::assets {
class AssetSystem;
}

namespace onyx::ecs {
template <>
class DependantFunctionArg< rhi::FrameContext > {
  public:
    static rhi::FrameContext& Get( const ECSExecutionContext& context );
};

template <>
class DependantFunctionArg< physics::PhysicsWorld > {
  public:
    static physics::PhysicsWorld& Get( const ECSExecutionContext& context );
};

template <>
class DependantFunctionArg< graphics::DebugDrawQueue > {
  public:
    static graphics::DebugDrawQueue& Get( const ECSExecutionContext& context );
};
} // namespace onyx::ecs

namespace onyx::game_core {
class GameCoreSystem : public IEngineSystem {
  public:
    static constexpr StringId32 TypeId = "onyx::game_core::GameCoreSystem";
    StringId32 getTypeId() const override { return TypeId; }

    GameCoreSystem();

    void update( DeltaGameTime deltaTime, rhi::GraphicsSystem& graphicsSystem, IEngine& engine );

    void setScene( assets::AssetHandle< Scene >& scene ) { m_scene = scene; }
    assets::AssetHandle< Scene >& getScene() { return m_scene; }
    const assets::AssetHandle< Scene >& getScene() const { return m_scene; }

    ecs::ComponentFactory& getComponentFactory() { return m_componentFactory; }
    const ecs::ComponentFactory& getComponentFactory() const { return m_componentFactory; }

    ecs::EcsBuilder getEcsBuilder() { return { m_componentFactory, m_ecsGraph }; }

  private:
    assets::AssetHandle< Scene > m_scene;
    ecs::ComponentFactory m_componentFactory;
    ecs::EntityComponentSystemsGraph m_ecsGraph;
};
} // namespace onyx::game_core
