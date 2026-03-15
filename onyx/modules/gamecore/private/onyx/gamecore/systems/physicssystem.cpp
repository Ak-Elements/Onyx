#include <onyx/gamecore/systems/physicssystem.h>

#include <onyx/graphics/debug/debugdrawqueue.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/entity/entitycommandbuffer.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/components/physics/boxcollidercomponent.gen.h>
#include <onyx/gamecore/components/physics/spherecollidercomponent.gen.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/physics/physicssystem.h>

#include <onyx/gamecore/scene/scene.h>

namespace Onyx::GameCore::Physics
{
    struct InitPhysics {};
    struct DynamicBody {};

    namespace Init
    {
        void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, BoxColliderComponent&& boxCollider)
        {
            registry.AddComponent<BoxColliderComponent>(entity, boxCollider);
            registry.AddComponent<InitPhysics>(entity);
        }

        void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, SphereColliderComponent&& sphereCollider)
        {
            registry.AddComponent<SphereColliderComponent>(entity, sphereCollider);
            registry.AddComponent<InitPhysics>(entity);
        }
    }
    
    namespace StreamIn
    {
        using Access = Entity::Access
            ::Read<TransformComponent>
            ::ReadIfExists<SphereColliderComponent, BoxColliderComponent>
            ::With<InitPhysics>;
        using PhysiscsEntity = Access::AsEntity;

        void system(PhysiscsEntity entity, Onyx::Physics::PhysicsWorld& physicsSystem, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [transform] = entity;

            Onyx::Physics::PhysicsBodyId bodyId;
            Onyx::Physics::MotionType motionType = Onyx::Physics::MotionType::Static;
            if( const BoxColliderComponent* boxCollider = entity.TryGetComponent<BoxColliderComponent>() )
            {
                motionType = boxCollider->MotionType;
                bodyId = physicsSystem.CreateBoxCollider(transform.Translation, transform.Rotation, boxCollider->HalfExtents, boxCollider->MotionType, boxCollider->Layer);
            }
            else if( const SphereColliderComponent* sphereCollider = entity.TryGetComponent<SphereColliderComponent>() )
            {
                motionType = sphereCollider->MotionType;
                bodyId = physicsSystem.CreateSphereCollider(transform.Translation, transform.Rotation, sphereCollider->Radius, sphereCollider->MotionType, sphereCollider->Layer);
            }

            entityCommandBuffer.AddComponent<Components::PhysicsBodyId>(entity, bodyId);
            if (motionType == Onyx::Physics::MotionType::Dynamic)
            {
                entityCommandBuffer.AddComponent<DynamicBody>(entity);
            }

            entityCommandBuffer.RemoveComponent<InitPhysics>(entity);
        }
    }

    namespace Simulate
    {
        void system(Onyx::Physics::PhysicsWorld& physicsSystem)
        {
            physicsSystem.Update();
        }
    }

    namespace PostPhysics
    {
        using Access = Entity::Access
            ::Read<Components::PhysicsBodyId>
            ::Write<TransformComponent>
            ::With<DynamicBody>;

        using PhysiscsEntity = Access::AsEntity;

        void system(PhysiscsEntity entity, Onyx::Physics::PhysicsWorld& physicsSystem)
        {
            auto&& [physicsBody, transform] = entity;
            transform.Translation = physicsSystem.GetPosition(physicsBody.BodyId);
        }
    }

    namespace DebugDraw
    {
        using Access = Entity::Access
            ::Read<TransformComponent>
            ::ReadIfExists<SphereColliderComponent, BoxColliderComponent>
            ::With<Components::PhysicsBodyId>;

        using ColliderEntity = Access::AsEntity;

        void system(ColliderEntity entity, Onyx::Graphics::DebugDrawQueue& debugDraw)
        {
            auto&& [ transform ] = entity;

            if( const SphereColliderComponent* sphereCollider = entity.TryGetComponent<SphereColliderComponent>() )
            {
                debugDraw.addWireframeSphere(transform.Translation, sphereCollider->Radius, 0xFF0000FF);
            }
            if( const BoxColliderComponent* boxCollider = entity.TryGetComponent<BoxColliderComponent>() )
            {
                debugDraw.addWireframeBox(transform.Translation, boxCollider->HalfExtents, transform.Rotation.ToMatrix3(), 0xFF0000FF);
            }
        }
    }

    void registerSystems(Entity::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterSystem(StreamIn::system);
        ecsBuilder.RegisterSystem(Simulate::system);
        ecsBuilder.RegisterSystem(PostPhysics::system);

        ecsBuilder.RegisterSystem(DebugDraw::system);

        ecsBuilder.RegisterComponent<BoxColliderComponent>(Init::factory);
        ecsBuilder.RegisterComponent<SphereColliderComponent>(Init::factory);
    }
}
