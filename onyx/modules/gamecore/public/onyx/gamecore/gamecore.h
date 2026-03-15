#pragma once
#include <onyx/engine/enginesystem.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/componentfactory.h>
#include <onyx/entity/ecsbuilder.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/debug/debugdrawqueue.h>

namespace Onyx::Graphics
{
    class GraphicsSystem;
}

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Entity
{
    template <>
    class DependantFunctionArg<Graphics::FrameContext>
    {
    public:
        static Graphics::FrameContext& Get(const ECSExecutionContext& context);
    };
    
    template <>
    class DependantFunctionArg<Physics::PhysicsWorld>
    {
    public:
        static Physics::PhysicsWorld& Get(const ECSExecutionContext& context);
    };

    template <>
    class DependantFunctionArg<Graphics::DebugDrawQueue>
    {
    public:
        static Graphics::DebugDrawQueue& Get(const ECSExecutionContext& context);
    };
}

namespace Onyx::GameCore
{
    class GameCoreSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::GameCore::GameCoreModule";
        StringId32 GetTypeId() const override { return TypeId; }

        GameCoreSystem();
        
        void Update(DeltaGameTime deltaTime, Graphics::GraphicsSystem& graphicsSystem, IEngine& engine);

        void SetScene(Assets::AssetHandle<Scene>& scene) { m_Scene = scene; }
        Assets::AssetHandle<Scene>& GetScene() { return m_Scene; }
        const Assets::AssetHandle<Scene>& GetScene() const { return m_Scene; }

        Entity::ComponentFactory& GetComponentFactory() { return m_ComponentFactory; }
        const Entity::ComponentFactory& GetComponentFactory() const { return m_ComponentFactory; }

        Entity::EcsBuilder GetEcsBuilder() { return { m_ComponentFactory, m_ECSGraph }; }

    private:
        Assets::AssetHandle<Scene> m_Scene;
        Entity::ComponentFactory m_ComponentFactory;
        Entity::EntityComponentSystemsGraph m_ECSGraph;
    };
}
