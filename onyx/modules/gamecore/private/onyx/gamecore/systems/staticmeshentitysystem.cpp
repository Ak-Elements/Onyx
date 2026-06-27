#include <onyx/gamecore/systems/staticmeshentitysystem.h>

#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/graphics/materialcomponent.gen.h>
#include <onyx/gamecore/components/graphics/staticmeshcomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entitycommandbuffer.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/scene/sceneframedata.h>

namespace onyx::game_core::static_mesh {
struct LoadMesh {};

namespace Init {
void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, StaticMeshComponent&& staticMeshComponent ) {
    registry.AddComponent< LoadMesh >( entity );
    registry.AddComponent< StaticMeshComponent >( entity, staticMeshComponent );
}
} // namespace Init

namespace StreamIn {
using Access = ecs::Access ::Write< StaticMeshComponent >::With< LoadMesh >;

using MeshEntity = Access::AsEntity;

void system( MeshEntity entity, onyx::assets::AssetSystem& assetSystem, ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ staticMesh ] = entity;

    assetSystem.getAsset( staticMesh.Mesh.getId(), staticMesh.Mesh );
    entityCommandBuffer.RemoveComponent< LoadMesh >( entity );
}
} // namespace StreamIn

namespace QueueRender {
using Access = ecs::Access ::Read< StaticMeshComponent, TransformComponent >::Write< MaterialComponent >;

using MeshEntity = Access::AsEntity;

void system( MeshEntity entity, rhi::FrameContext& frameContext, assets::AssetSystem& assetSystem ) {
    game_core::SceneFrameData& sceneFrameData = static_cast< game_core::SceneFrameData& >( *frameContext.FrameData );

    auto&& [ staticMesh, transform, materialComponent ] = entity;

    if( !staticMesh.Mesh.isLoaded() )
        return;

    if( staticMesh.Mesh->getVertices().empty() )
        return;

    if( materialComponent.Material.hasAssetId() == false )
        return;

    if( materialComponent.Material.hasHandle() == false ) {
        if( materialComponent.Material.hasAssetId() ) {
            // this should be moved to the component create
            assetSystem.getAsset( materialComponent.Material.getId(), materialComponent.Material );
            return;
        }
    }

    if( ( materialComponent.Material.isValid() == false ) )
        return;

    game_core::StaticMeshDrawCall& drawCall = sceneFrameData.m_StaticMeshDrawCalls.emplace_back();
    drawCall.Transforms.emplace_back( world_transform::getTransform( transform ) );
    drawCall.VertexData = staticMesh.Mesh->getVertexBuffer();
    drawCall.Indices = staticMesh.Mesh->getIndexBuffer();
    drawCall.Material = materialComponent.Material;
}
} // namespace QueueRender

void registerSystems( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterSystem( StreamIn::system );
    ecsBuilder.RegisterSystem( QueueRender::system );

    ecsBuilder.RegisterComponent< StaticMeshComponent >( Init::factory );
}
} // namespace onyx::game_core::static_mesh
