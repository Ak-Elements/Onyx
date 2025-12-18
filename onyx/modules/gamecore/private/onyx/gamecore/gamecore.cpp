#include <onyx/gamecore/gamecore.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/components/camera.gen.h>
#include <onyx/gamecore/components/cameracomponent.h>
#include <onyx/graphics/graphicssystem.h>
#include <onyx/gamecore/components/graphics/lightcomponents.h>
#include <onyx/gamecore/components/graphics/materialcomponent.h>
#include <onyx/gamecore/components/graphics/textcomponent.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/serialize/sceneserializer.h>
#include <onyx/gamecore/systems/lightingsystem.h>
#include <onyx/gamecore/components/freecameracomponent.h>
#include <onyx/gamecore/components/idcomponent.h>
#include <onyx/gamecore/components/namecomponent.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/rendertasks/depthprepassrendertask.h>
#include <onyx/gamecore/rendertasks/staticmeshrendertask.h>
#include <onyx/gamecore/rendertasks/textrendertask.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/graphics/graphicssystem.h>
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
                textComponent.SetFont(fontAsset);

                registry.AddComponent<TextComponent>(entity, std::move(textComponent));
            });

            ecsBuilder.RegisterComponent<CameraComponent>();
        }

        void RegisterEntitySystems(Entity::EcsBuilder& ecsBuilder)
        {
            FreeCamera::registerSystems(ecsBuilder);
            ecsBuilder.RegisterSystem(Camera::system);

            Lighting::registerSystems(ecsBuilder);
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
        if ((m_Scene.IsValid() == false) || m_Scene->IsLoading())
        {
            return;
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

Onyx::Graphics::FrameContext& Onyx::Entity::DependentFunctionArg<Onyx::Graphics::FrameContext&>::Get(const ECSExecutionContext& context)
{
    Graphics::GraphicsSystem& graphicsSystem = context.Engine.GetSystem<Graphics::GraphicsSystem>();
    return graphicsSystem.GetFrameContext();
}
