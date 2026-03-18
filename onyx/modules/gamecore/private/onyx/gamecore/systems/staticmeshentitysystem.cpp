#include <onyx/gamecore/systems/staticmeshentitysystem.h>

#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/graphics/staticmeshcomponent.gen.h>
#include <onyx/gamecore/components/graphics/materialcomponent.gen.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/entity/entitycommandbuffer.h>

#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/scene/sceneframedata.h>

namespace onyx::game_core::static_mesh
{
    struct LoadMesh {};

    namespace Init
    {
        void factory(ecs::EntityRegistry& registry, ecs::EntityId entity, StaticMeshComponent&& staticMeshComponent)
        {
            registry.AddComponent<LoadMesh>(entity);
            registry.AddComponent<StaticMeshComponent>(entity, staticMeshComponent);
        }
    }
    
    namespace StreamIn
    {
        using Access = ecs::Access
            ::Write<StaticMeshComponent>
            ::With<LoadMesh>;

        using MeshEntity = Access::AsEntity;

        void system(MeshEntity entity, onyx::assets::AssetSystem& assetSystem, ecs::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [staticMesh] = entity;

            assetSystem.GetAsset(staticMesh.Mesh.GetId(), staticMesh.Mesh);
            entityCommandBuffer.RemoveComponent<LoadMesh>(entity);
        }
    }

    namespace QueueRender
    {
        using Access = ecs::Access
            ::Read<StaticMeshComponent, TransformComponent>
            ::Write<MaterialComponent>;

        using MeshEntity = Access::AsEntity;

        void system(MeshEntity entity, rhi::FrameContext& frameContext, assets::AssetSystem& assetSystem)
        {
            game_core::SceneFrameData& sceneFrameData = static_cast<game_core::SceneFrameData&>(*frameContext.FrameData);

            auto&& [staticMesh, transform, materialComponent] = entity;

            if( !staticMesh.Mesh.IsLoaded() )
                return;

            if (staticMesh.Mesh->GetVertices().empty())
                return;

            if ( materialComponent.Material.HasAssetId() == false)
                return;

            if ( materialComponent.Material.HasHandle() == false )
            {
                if( materialComponent.Material.HasAssetId() )
                {
                    // this should be moved to the component create
                    assetSystem.GetAsset(materialComponent.Material.GetId(), materialComponent.Material);
                    return;
                }
            }

            if ( ( materialComponent.Material.IsValid() == false ) )
                return;

            
            game_core::StaticMeshDrawCall& drawCall = sceneFrameData.m_StaticMeshDrawCalls.emplace_back();
            drawCall.Transforms.emplace_back(world_transform::GetTransform(transform));
            drawCall.VertexData = staticMesh.Mesh->GetVertexBuffer();
            drawCall.Indices = staticMesh.Mesh->GetIndexBuffer();
            drawCall.Material = materialComponent.Material;
        }
    }

    void registerSystems(ecs::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterSystem(StreamIn::system);
        ecsBuilder.RegisterSystem(QueueRender::system);

        ecsBuilder.RegisterComponent<StaticMeshComponent>(Init::factory);

    }
}
