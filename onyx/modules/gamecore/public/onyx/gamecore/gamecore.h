#pragma once
#include <onyx/engine/enginesystem.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/entity/componentfactory.h>
#include <onyx/entity/ecsbuilder.h>

#include <onyx/gamecore/scene/scene.h>

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
    class DependentFunctionArg<Graphics::GraphicsSystem&> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static Graphics::GraphicsSystem& Get(const ECSExecutionContext& context);
    };

    template <>
    class DependentFunctionArg<Assets::AssetSystem&> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static Assets::AssetSystem& Get(const ECSExecutionContext& context);
    };

    template <>
    class DependentFunctionArg<Graphics::FrameContext&> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static Graphics::FrameContext& Get(const ECSExecutionContext& context);
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

        void SetScene(Reference<Scene>& scene) { m_Scene = scene; }

        const Entity::ComponentFactory& GetComponentFactory() const { return m_ComponentFactory; }

        Entity::EcsBuilder GetEcsBuilder() { return { m_ComponentFactory, m_ECSGraph }; }

    private:
        
        Reference<Scene> m_Scene;
        Entity::ComponentFactory m_ComponentFactory;
        Entity::EntityComponentSystemsGraph m_ECSGraph;
    };
}
