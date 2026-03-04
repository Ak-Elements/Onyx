#include <onyx/gamecore/gamecore.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/graphics/directionallightcomponent.gen.h>
#include <onyx/gamecore/components/graphics/pointlightcomponent.gen.h>
#include <onyx/gamecore/components/graphics/spotlightcomponent.gen.h>
#include <onyx/gamecore/components/graphics/materialcomponent.gen.h>
#include <onyx/gamecore/components/graphics/textcomponent.gen.h>
#include <onyx/graphics/font/sdffont.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/serialize/sceneserializer.h>
#include <onyx/gamecore/systems/lightingsystem.h>
#include <onyx/gamecore/systems/camerasystem.h>
#include <onyx/gamecore/systems/physicssystem.h>
#include <onyx/gamecore/systems/staticmeshentitysystem.h>
#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/rendertasks/depthprepassrendertask.h>
#include <onyx/gamecore/rendertasks/staticmeshrendertask.h>
#include <onyx/gamecore/rendertasks/textrendertask.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/gamecore/systems/freecamerasystem.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
namespace Onyx::GameCore
{
    namespace GameCoreInit
    {
        void RegisterComponents(Entity::EcsBuilder& ecsBuilder)
        {
            ecsBuilder.RegisterComponent<IdComponent>();
            ecsBuilder.RegisterComponent<TransformComponent>();

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR
            ecsBuilder.RegisterComponent<NameComponent>();
#endif

            ecsBuilder.RegisterComponent<DirectionalLightComponent>();
            ecsBuilder.RegisterComponent<PointLightComponent>();
            ecsBuilder.RegisterComponent<SpotLightComponent>();
            ecsBuilder.RegisterComponent<MaterialComponent>();
            ecsBuilder.RegisterComponent<TextComponent>([](Entity::EntityRegistry& registry, Entity::EntityId entity, TextComponent&& textComponent)
                
            {
                Reference<Graphics::SDFFont> fontAsset;
                //loc_AssetSystem->GetAsset(textComponent.FontId, fontAsset);
                //textComponent.SetFont(fontAsset);

                registry.AddComponent<TextComponent>(entity, std::move(textComponent));
            });

            ecsBuilder.RegisterComponent<CameraComponent>();
        }

        void RegisterEntitySystems(Entity::EcsBuilder& ecsBuilder)
        {
            FreeCamera::registerSystems(ecsBuilder);
            Camera::registerSystems(ecsBuilder);

            Lighting::registerSystems(ecsBuilder);

            StaticMeshEntitySystem::registerSystems(ecsBuilder);

            Physics::registerSystems(ecsBuilder);
        }
    }

    GameCoreSystem::GameCoreSystem()
    {
        Entity::EcsBuilder ecsBuilder{ m_ComponentFactory, m_ECSGraph };
        GameCoreInit::RegisterComponents(ecsBuilder);
        GameCoreInit::RegisterEntitySystems(ecsBuilder);
    }

    void GameCoreSystem::Update(DeltaGameTime deltaTime, Graphics::GraphicsSystem& graphicsSystem, IEngine& engine)
    {
        if (m_Scene.IsLoaded() == false)
        {
            return;
        }

        if (m_Scene->GetRenderGraphRef().HasAssetId())
        {
            Graphics::RenderGraph& sceneRenderGraph = m_Scene->GetRenderGraph();
            if (sceneRenderGraph.IsLoaded() && sceneRenderGraph.IsInitialized() == false)
            {
                sceneRenderGraph.Init(graphicsSystem);
            }
        }

        // TODO: Can we find a cleaner / better solution for this?
        Graphics::FrameContext& frameContext = graphicsSystem.GetFrameContext();
        if (frameContext.FrameData == nullptr)
            frameContext.FrameData = MakeUnique<SceneFrameData>();

        SceneFrameData& sceneFrameData = static_cast<SceneFrameData&>(*frameContext.FrameData);
        sceneFrameData.m_StaticMeshDrawCalls.clear();
        sceneFrameData.m_StaticMeshIndirectDrawCalls.clear();
        sceneFrameData.m_VoxelChunksToInit.clear();

        Entity::ECSExecutionContext context { deltaTime, m_Scene->GetRegistry(), engine };
        m_ECSGraph.Update(context);
    }
}

Onyx::Graphics::GraphicsSystem& Onyx::Entity::DependentFunctionArg<Onyx::Graphics::GraphicsSystem&>::Get(const ECSExecutionContext& context)
{
    return context.Engine.GetSystem<Graphics::GraphicsSystem>();
}

Onyx::Assets::AssetSystem& Onyx::Entity::DependentFunctionArg<Onyx::Assets::AssetSystem&>::Get(const ECSExecutionContext& context)
{
    return context.Engine.GetSystem<Assets::AssetSystem>();
}

Onyx::Physics::PhysicsWorld& Onyx::Entity::DependentFunctionArg<Onyx::Physics::PhysicsWorld&>::Get(const ECSExecutionContext& context)
{
    GameCore::GameCoreSystem& gameCoreSystem = context.Engine.GetSystem<GameCore::GameCoreSystem>();
    return gameCoreSystem.GetScene()->GetPhysicsWorld();
}

Onyx::Graphics::FrameContext& Onyx::Entity::DependentFunctionArg<Onyx::Graphics::FrameContext&>::Get(const ECSExecutionContext& context)
{
    Graphics::GraphicsSystem& graphicsSystem = context.Engine.GetSystem<Graphics::GraphicsSystem>();
    return graphicsSystem.GetFrameContext();
}
