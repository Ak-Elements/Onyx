#pragma once
#include <onyx/engine/enginesystem.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/entity/componentfactory.h>
#include <onyx/entity/ecsbuilder.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Entity
{
    template <>
    class DependentFunctionArg<Graphics::GraphicsApi&> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static Graphics::GraphicsApi& Get(const ECSExecutionContext& context);
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
        void Init(Assets::AssetSystem& assetSystem);

        void Update(DeltaGameTime deltaTime, Graphics::GraphicsApi& graphicsApi, IEngine& engine);

        void SetScene(Reference<Scene>& scene) { Scene = scene; }

        const Entity::ComponentFactory& GetComponentFactory() const { return m_ComponentFactory; }

        Entity::EcsBuilder GetEcsBuilder() { return { m_ComponentFactory, m_ECSGraph }; }

    private:
        
        Reference<Scene> Scene;
        Entity::ComponentFactory m_ComponentFactory;
        Entity::EntityComponentSystemsGraph m_ECSGraph;
    };
}
