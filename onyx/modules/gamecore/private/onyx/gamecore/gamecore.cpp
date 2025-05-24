#include <onyx/gamecore/gamecore.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/assets/sdffont.h>
#include <onyx/gamecore/components/cameracomponent.h>

#include <onyx/gamecore/components/graphics/lightcomponents.h>
#include <onyx/gamecore/components/graphics/materialcomponent.h>
#include <onyx/gamecore/components/graphics/textcomponent.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/serialize/sceneserializer.h>
#include <onyx/gamecore/serialize/sdffontserializer.h>
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

namespace
{
    Onyx::Assets::AssetSystem* loc_AssetSystem = nullptr;
    Onyx::Graphics::GraphicsApi* loc_GraphicsApi = nullptr;
}


namespace Onyx::GameCore
{
    namespace GameCoreInit
    {
        void RegisterAssets(Assets::AssetSystem& assetSystem)
        {
            Assets::AssetSystem::Register<Scene, SceneSerializer>(assetSystem);
            Assets::AssetSystem::Register<SDFFont, SDFFontSerializer>(assetSystem);
        }

        void RegisterComponents()
        {
            Entity::EntityRegistry::RegisterComponent<IdComponent>();
            Entity::EntityRegistry::RegisterComponent<TransformComponent>();

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR
            Entity::EntityRegistry::RegisterComponent<NameComponent>();
#endif

            Entity::EntityRegistry::RegisterComponent<DirectionalLightComponent>();
            Entity::EntityRegistry::RegisterComponent<PointLightComponent>();
            Entity::EntityRegistry::RegisterComponent<SpotLightComponent>();
            Entity::EntityRegistry::RegisterComponent<MaterialComponent>();
            Entity::EntityRegistry::RegisterComponent<TextComponent>();

            Entity::EntityRegistry::RegisterComponent<CameraComponent>();
            Entity::EntityRegistry::RegisterComponent<FreeCameraControllerComponent>();
            Entity::EntityRegistry::Factory<FreeCameraRuntimeComponent>();
            Entity::EntityRegistry::RegisterComponent<FreeCameraRuntimeComponent>();
        }

        void RegisterEntitySystems(Entity::EntityComponentSystemsGraph& ecsGraph)
        {
            FreeCamera::registerSystems(ecsGraph);
            ecsGraph.Register(Camera::system);

            Lighting::registerSystems(ecsGraph);
        }
    }
  
    void GameCoreSystem::Init(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi)
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<DepthPrePassRenderGraphNode>("Scene/Depth Pre Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<StaticMeshRenderGraphNode>("Graphics/Static Mesh Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<MSDFFontRenderPass>("Scene/3D MSDF Font Pass");

        GameCoreInit::RegisterAssets(assetSystem);

        GameCoreInit::RegisterComponents();

        GameCoreInit::RegisterEntitySystems(m_ECSGraph);

        loc_AssetSystem = &assetSystem;
        loc_GraphicsApi = &graphicsApi;
    }

    void GameCoreSystem::Update(DeltaGameTime deltaTime, Graphics::GraphicsApi& graphicsApi, IEngine& engine)
    {
        if ((Scene.IsValid() == false) || Scene->IsLoading())
        {
            return;
        }

        // TODO: Can we find a cleaner / better solution for this?
        Graphics::FrameContext& frameContext = graphicsApi.GetFrameContext();
        if (frameContext.FrameData == nullptr)
            frameContext.FrameData = MakeUnique<SceneFrameData>();

        SceneFrameData& sceneFrameData = static_cast<SceneFrameData&>(*frameContext.FrameData);
        sceneFrameData.m_StaticMeshDrawCalls.clear();

        Entity::ECSExecutionContext context { deltaTime, Scene->GetRegistry(), engine };
        m_ECSGraph.Update(context);
    }
}

Onyx::Graphics::GraphicsApi& Onyx::Entity::DependentFunctionArg<Onyx::Graphics::GraphicsApi&>::Get(const ECSExecutionContext& /*context*/)
{
    return *loc_GraphicsApi;
}

Onyx::Assets::AssetSystem& Onyx::Entity::DependentFunctionArg<Onyx::Assets::AssetSystem&>::Get(const ECSExecutionContext& /*context*/)
{
    return *loc_AssetSystem;
}

Onyx::Graphics::FrameContext& Onyx::Entity::DependentFunctionArg<Onyx::Graphics::FrameContext&>::Get(const ECSExecutionContext& /*context*/)
{
    return loc_GraphicsApi->GetFrameContext();
}
