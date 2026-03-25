#include <onyx/gamecore/gamecore.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/components/cameracomponent.gen.h>
#include <onyx/gamecore/components/graphics/directionallightcomponent.gen.h>
#include <onyx/gamecore/components/graphics/materialcomponent.gen.h>
#include <onyx/gamecore/components/graphics/pointlightcomponent.gen.h>
#include <onyx/gamecore/components/graphics/spotlightcomponent.gen.h>
#include <onyx/gamecore/components/graphics/textcomponent.gen.h>
#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/rendertasks/depthprepassrendertask.h>
#include <onyx/gamecore/rendertasks/staticmeshrendertask.h>
#include <onyx/gamecore/rendertasks/textrendertask.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/gamecore/serialize/sceneserializer.h>
#include <onyx/gamecore/systems/camerasystem.h>
#include <onyx/gamecore/systems/freecamerasystem.h>
#include <onyx/gamecore/systems/lightingsystem.h>
#include <onyx/gamecore/systems/physicssystem.h>
#include <onyx/gamecore/systems/staticmeshentitysystem.h>
#include <onyx/graphics/font/sdffont.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/rhi/graphicssystem.h>

namespace onyx::game_core {
namespace GameCoreInit {
void RegisterComponents( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterComponent< IdComponent >();
    ecsBuilder.RegisterComponent< TransformComponent >(
        []( ecs::EntityRegistry& registry, ecs::EntityId entity, TransformComponent&& transform ) {
            transform.Rotation = Rotor3f32::fromEulerAngles( transform.RotationEuler );
            registry.AddComponent< TransformComponent >( entity, std::move( transform ) );
        } );

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR
    ecsBuilder.RegisterComponent< NameComponent >();
#endif

    ecsBuilder.RegisterComponent< DirectionalLightComponent >();
    ecsBuilder.RegisterComponent< PointLightComponent >();
    ecsBuilder.RegisterComponent< SpotLightComponent >();
    ecsBuilder.RegisterComponent< MaterialComponent >();
    ecsBuilder.RegisterComponent< TextComponent >(
        []( ecs::EntityRegistry& registry, ecs::EntityId entity, TextComponent&& textComponent ) {
            Reference< graphics::SDFFont > fontAsset;
            // loc_AssetSystem->GetAsset(textComponent.FontId, fontAsset);
            // textComponent.SetFont(fontAsset);

            registry.AddComponent< TextComponent >( entity, std::move( textComponent ) );
        } );

    ecsBuilder.RegisterComponent< CameraComponent >();
}

void RegisterEntitySystems( ecs::EcsBuilder& ecsBuilder ) {
    FreeCamera::registerSystems( ecsBuilder );
    camera::registerSystems( ecsBuilder );

    lighting::registerSystems( ecsBuilder );

    static_mesh::registerSystems( ecsBuilder );

    physics::registerSystems( ecsBuilder );
}
} // namespace GameCoreInit

GameCoreSystem::GameCoreSystem() {
    ecs::EcsBuilder ecsBuilder{ m_ComponentFactory, m_ECSGraph };
    GameCoreInit::RegisterComponents( ecsBuilder );
    GameCoreInit::RegisterEntitySystems( ecsBuilder );
}

void GameCoreSystem::update( DeltaGameTime deltaTime, rhi::GraphicsSystem& graphicsSystem, IEngine& engine ) {
    if ( m_Scene.isLoaded() == false ) {
        return;
    }

    if ( m_Scene->GetRenderGraphRef().hasAssetId() ) {
        graphics::RenderGraph& sceneRenderGraph = m_Scene->GetRenderGraph();
        if ( sceneRenderGraph.isLoaded() && sceneRenderGraph.IsInitialized() == false ) {
            sceneRenderGraph.Init( graphicsSystem );
        }
    }

    // TODO: Can we find a cleaner / better solution for this?
    rhi::FrameContext& frameContext = graphicsSystem.GetFrameContext();
    if ( frameContext.FrameData == nullptr )
        frameContext.FrameData = makeUnique< SceneFrameData >();

    SceneFrameData& sceneFrameData = static_cast< SceneFrameData& >( *frameContext.FrameData );
    sceneFrameData.m_StaticMeshDrawCalls.clear();
    sceneFrameData.m_StaticMeshIndirectDrawCalls.clear();
    sceneFrameData.m_VoxelChunksToInit.clear();

    ecs::ECSExecutionContext context{ deltaTime, m_Scene->GetRegistry(), engine };
    m_ECSGraph.Update( context );
}
} // namespace onyx::game_core

onyx::physics::PhysicsWorld& onyx::ecs::DependantFunctionArg< onyx::physics::PhysicsWorld >::Get(
    const ECSExecutionContext& context ) {
    game_core::GameCoreSystem& gameCoreSystem = context.Engine.getSystem< game_core::GameCoreSystem >();
    return gameCoreSystem.GetScene()->GetPhysicsWorld();
}

onyx::rhi::FrameContext& onyx::ecs::DependantFunctionArg< onyx::rhi::FrameContext >::Get(
    const ECSExecutionContext& context ) {
    rhi::GraphicsSystem& graphicsSystem = context.Engine.getSystem< rhi::GraphicsSystem >();
    return graphicsSystem.GetFrameContext();
}

onyx::graphics::DebugDrawQueue& onyx::ecs::DependantFunctionArg< onyx::graphics::DebugDrawQueue >::Get(
    const ECSExecutionContext& context ) {
    game_core::GameCoreSystem& gameCoreSystem = context.Engine.getSystem< game_core::GameCoreSystem >();
    assets::AssetHandle< game_core::Scene >& activeScene = gameCoreSystem.GetScene();
    graphics::RenderGraph& renderGraph = activeScene->GetRenderGraph();
    graphics::DebugDrawQueue& debugQueue = renderGraph.GetInput< graphics::DebugDrawQueue >();
    return debugQueue;
}
