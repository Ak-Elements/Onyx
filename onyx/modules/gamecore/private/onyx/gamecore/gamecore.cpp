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
void registerComponents( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterComponent< IdComponent >();
    ecsBuilder.RegisterComponent< TransformComponent >(
        []( ecs::EntityRegistry& registry, ecs::EntityId entity, TransformComponent&& transform ) {
            transform.Rotation = Rotor3f32( transform.RotationEuler );
            registry.AddComponent< TransformComponent >( entity, transform );
        } );

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR
    ecsBuilder.RegisterComponent< NameComponent >();
#endif

    ecsBuilder.RegisterComponent< DirectionalLightComponent >();
    ecsBuilder.RegisterComponent< PointLightComponent >();
    ecsBuilder.RegisterComponent< SpotLightComponent >();
    ecsBuilder.RegisterComponent< MaterialComponent >();
    ecsBuilder.RegisterComponent< TextComponent >();
    ecsBuilder.RegisterComponent< CameraComponent >();
}

void registerEntitySystems( ecs::EcsBuilder& ecsBuilder ) {
    free_camera::registerSystems( ecsBuilder );
    camera::registerSystems( ecsBuilder );

    lighting::registerSystems( ecsBuilder );

    static_mesh::registerSystems( ecsBuilder );

    physics::registerSystems( ecsBuilder );
}
} // namespace GameCoreInit

GameCoreSystem::GameCoreSystem() {
    ecs::EcsBuilder ecsBuilder{ m_componentFactory, m_ecsGraph };
    GameCoreInit::registerComponents( ecsBuilder );
    GameCoreInit::registerEntitySystems( ecsBuilder );
}

void GameCoreSystem::update( DeltaGameTime deltaTime, rhi::GraphicsSystem& graphicsSystem, IEngine& engine ) {
    if( m_scene.isLoaded() == false ) {
        return;
    }

    if( m_scene->getRenderGraphRef().hasAssetId() ) {
        graphics::RenderGraph& sceneRenderGraph = m_scene->getRenderGraph();
        if( sceneRenderGraph.isLoaded() && sceneRenderGraph.isInitialized() == false ) {
            sceneRenderGraph.init( graphicsSystem );
        }
    }

    // TODO: Can we find a cleaner / better solution for this?
    rhi::FrameContext& frameContext = graphicsSystem.getFrameContext();
    if( frameContext.FrameData == nullptr )
        frameContext.FrameData = makeUnique< SceneFrameData >();

    SceneFrameData& sceneFrameData = static_cast< SceneFrameData& >( *frameContext.FrameData );
    sceneFrameData.m_StaticMeshDrawCalls.clear();
    sceneFrameData.m_StaticMeshIndirectDrawCalls.clear();
    sceneFrameData.m_VoxelChunksToInit.clear();

    ecs::ECSExecutionContext context{ deltaTime, m_scene->getRegistry(), engine };
    m_ecsGraph.Update( context );
}
} // namespace onyx::game_core

onyx::physics::PhysicsWorld& onyx::ecs::DependantFunctionArg< onyx::physics::PhysicsWorld >::Get(
    const ECSExecutionContext& context ) {
    game_core::GameCoreSystem& gameCoreSystem = context.Engine.getSystem< game_core::GameCoreSystem >();
    return gameCoreSystem.getScene()->getPhysicsWorld();
}

onyx::rhi::FrameContext& onyx::ecs::DependantFunctionArg< onyx::rhi::FrameContext >::Get(
    const ECSExecutionContext& context ) {
    rhi::GraphicsSystem& graphicsSystem = context.Engine.getSystem< rhi::GraphicsSystem >();
    return graphicsSystem.getFrameContext();
}

onyx::graphics::DebugDrawQueue& onyx::ecs::DependantFunctionArg< onyx::graphics::DebugDrawQueue >::Get(
    const ECSExecutionContext& context ) {
    game_core::GameCoreSystem& gameCoreSystem = context.Engine.getSystem< game_core::GameCoreSystem >();
    assets::AssetHandle< game_core::Scene >& activeScene = gameCoreSystem.getScene();
    graphics::RenderGraph& renderGraph = activeScene->getRenderGraph();
    graphics::DebugDrawQueue& debugQueue = renderGraph.getInput< graphics::DebugDrawQueue >();
    return debugQueue;
}
