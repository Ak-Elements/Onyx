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
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>

namespace Onyx::GameCore
{
    namespace 
    {
        Assets::AssetSystem* loc_AssetSystem = nullptr;
        Graphics::GraphicsApi* loc_GraphicsApi = nullptr;
    }

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
            Entity::EntityRegistry::RegisterComponent<FreeCameraRuntimeComponent>();
        }

        void RegisterEntitySystems(DynamicArray<InplaceFunction<void(onyxU64, Scene&, Graphics::GraphicsApi&, Assets::AssetSystem&)>>& gameLoop)
        {
            gameLoop.emplace_back(FreeCamera::system);
            gameLoop.emplace_back(Camera::system);
            gameLoop.emplace_back(Lighting::system);
        }
    }
  
    void GameCoreSystem::Init(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi)
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<DepthPrePassRenderGraphNode>("Scene/Depth Pre Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<StaticMeshRenderGraphNode>("Graphics/Static Mesh Pass");
        Graphics::RenderGraphNodeFactory::RegisterNode<MSDFFontRenderPass>("Scene/3D MSDF Font Pass");

        GameCoreInit::RegisterAssets(assetSystem);

        GameCoreInit::RegisterComponents();

        GameCoreInit::RegisterEntitySystems(GameLoop);

        loc_AssetSystem = &assetSystem;
        loc_GraphicsApi = &graphicsApi;
    }

    void GameCoreSystem::Update(uint64_t deltaTime)
    {
        if (Scene.IsValid() == false)
        {
            return;
        }

        for (auto& gameLoopFunctor : GameLoop)
        {
            gameLoopFunctor(deltaTime, *Scene, *loc_GraphicsApi, *loc_AssetSystem);
        }
    }
}
