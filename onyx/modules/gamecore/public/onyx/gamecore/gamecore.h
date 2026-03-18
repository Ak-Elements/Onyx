#pragma once
#include <onyx/engine/enginesystem.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/componentfactory.h>
#include <onyx/entity/ecsbuilder.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/debug/debugdrawqueue.h>

namespace onyx::rhi
{
    class GraphicsSystem;
}

namespace onyx::assets
{
    class AssetSystem;
}

namespace onyx::ecs
{
    template <>
    class DependantFunctionArg<rhi::FrameContext>
    {
    public:
        static rhi::FrameContext& Get(const ECSExecutionContext& context);
    };
    
    template <>
    class DependantFunctionArg<physics::PhysicsWorld>
    {
    public:
        static physics::PhysicsWorld& Get(const ECSExecutionContext& context);
    };

    template <>
    class DependantFunctionArg<graphics::DebugDrawQueue>
    {
    public:
        static graphics::DebugDrawQueue& Get(const ECSExecutionContext& context);
    };
}

namespace onyx::game_core
{
    class GameCoreSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "onyx::game_core::GameCoreSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        GameCoreSystem();
        
        void Update(DeltaGameTime deltaTime, rhi::GraphicsSystem& graphicsSystem, IEngine& engine);

        void SetScene(assets::AssetHandle<Scene>& scene) { m_Scene = scene; }
        assets::AssetHandle<Scene>& GetScene() { return m_Scene; }
        const assets::AssetHandle<Scene>& GetScene() const { return m_Scene; }

        ecs::ComponentFactory& GetComponentFactory() { return m_ComponentFactory; }
        const ecs::ComponentFactory& GetComponentFactory() const { return m_ComponentFactory; }

        ecs::EcsBuilder GetEcsBuilder() { return { m_ComponentFactory, m_ECSGraph }; }

    private:
        assets::AssetHandle<Scene> m_Scene;
        ecs::ComponentFactory m_ComponentFactory;
        ecs::EntityComponentSystemsGraph m_ECSGraph;
    };
}
