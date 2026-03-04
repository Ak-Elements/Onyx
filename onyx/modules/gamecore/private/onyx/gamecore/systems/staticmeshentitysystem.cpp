#include <onyx/gamecore/systems/staticmeshentitysystem.h>

#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/graphics/staticmeshcomponent.gen.h>
#include <onyx/gamecore/components/graphics/materialcomponent.gen.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/assets/assetsystem.h>

#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/scene/sceneframedata.h>

namespace Onyx::GameCore::StaticMeshEntitySystem
{
    struct LoadMesh {};

    namespace Init
    {
        void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, StaticMeshComponent&& staticMeshComponent)
        {
            registry.AddComponent<LoadMesh>(entity);
            registry.AddComponent<StaticMeshComponent>(entity, staticMeshComponent);
        }
    }
    
    namespace StreamIn
    {
        using EntityAccess = Entity::Entity<StaticMeshComponent, LoadMesh>;
        void system(EntityAccess entity, Onyx::Assets::AssetSystem& assetSystem, Graphics::GraphicsSystem& graphicsSystem, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [staticMesh] = entity.Get();

            assetSystem.GetAsset(staticMesh.Mesh.GetId(), staticMesh.Mesh);
            //Onyx::Physics::PhysicsBodyId bodyId = physicsSystem.CreateBoxCollider(transform.Translation, transform.Rotation, boxCollider.HalfExtents, boxCollider.MotionType, boxCollider.Layer);
            //entityCommandBuffer.AddComponent<Components::PhysicsBodyId>(entity, bodyId);
            entityCommandBuffer.RemoveComponent<LoadMesh>(entity);
        }
    }

    namespace QueueRender
    {
        using VolumeEntityAccess = Entity::Entity<const StaticMeshComponent, const TransformComponent, GameCore::MaterialComponent>;
        void system(VolumeEntityAccess entity, Graphics::FrameContext& frameContext, Assets::AssetSystem& assetSystem)
        {
            GameCore::SceneFrameData& sceneFrameData = static_cast<GameCore::SceneFrameData&>(*frameContext.FrameData);

            auto&& [staticMesh, transform, materialComponent] = entity.Get();

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

            
            GameCore::StaticMeshDrawCall& drawCall = sceneFrameData.m_StaticMeshDrawCalls.emplace_back();
            drawCall.Transforms.emplace_back(WorldTransform::GetTransform(transform));
            drawCall.VertexData = staticMesh.Mesh->GetVertexBuffer();
            drawCall.Indices = staticMesh.Mesh->GetIndexBuffer();
            drawCall.Material = materialComponent.Material;
        }
    }

    void registerSystems(Entity::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterSystem(StreamIn::system);
        ecsBuilder.RegisterSystem(QueueRender::system);

        ecsBuilder.RegisterComponent<StaticMeshComponent>(Init::factory);

    }
}
