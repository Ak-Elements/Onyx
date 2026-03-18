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

namespace onyx::game_core::physics
{
    struct InitPhysics {};
    struct DynamicBody {};

    namespace Init
    {
        void factory(ecs::EntityRegistry& registry, ecs::EntityId entity, BoxColliderComponent&& boxCollider)
        {
            registry.AddComponent<BoxColliderComponent>(entity, boxCollider);
            registry.AddComponent<InitPhysics>(entity);
        }

        void factory(ecs::EntityRegistry& registry, ecs::EntityId entity, SphereColliderComponent&& sphereCollider)
        {
            registry.AddComponent<SphereColliderComponent>(entity, sphereCollider);
            registry.AddComponent<InitPhysics>(entity);
        }
    }
    
    namespace StreamIn
    {
        void initPhysicsEntity(ecs::EntityCommandBuffer& entityCommandBuffer, ecs::EntityId entity, onyx::physics::PhysicsBodyId bodyId, onyx::physics::MotionType motionType)
        {
            entityCommandBuffer.AddComponent<components::PhysicsBodyId>(entity, bodyId);
            if (motionType == onyx::physics::MotionType::Dynamic)
            {
                entityCommandBuffer.AddComponent<DynamicBody>(entity);
            }

            entityCommandBuffer.RemoveComponent<InitPhysics>(entity);
        }

        using SphereColliderAccess = ecs::Access
            ::Read<TransformComponent>
            ::Read<SphereColliderComponent>
            ::With<InitPhysics>;

        using SphereColliderEntity = SphereColliderAccess::AsEntity;

        void initSphereColliders(SphereColliderEntity entity, onyx::physics::PhysicsWorld& physicsSystem, ecs::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [transform, sphereCollider] = entity;

            onyx::physics::PhysicsBodyId bodyId = physicsSystem.CreateSphereCollider(transform.Translation,
                 transform.Rotation,
                 sphereCollider.Radius,
                 sphereCollider.MotionType,
                 sphereCollider.Layer);

            initPhysicsEntity(entityCommandBuffer, entity.GetId(), bodyId, sphereCollider.MotionType);
        }

        using BoxColliderAccess = ecs::Access
            ::Read<TransformComponent>
            ::Read<BoxColliderComponent>
            ::With<InitPhysics>;

        using BoxColliderEntity = BoxColliderAccess::AsEntity;

        void initBoxColliders(BoxColliderEntity entity, onyx::physics::PhysicsWorld& physicsSystem, ecs::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [transform, boxCollider] = entity;

            onyx::physics::PhysicsBodyId bodyId = physicsSystem.CreateBoxCollider(transform.Translation,
                 transform.Rotation,
                 boxCollider.HalfExtents,
                 boxCollider.MotionType,
                 boxCollider.Layer);

            initPhysicsEntity(entityCommandBuffer, entity.GetId(), bodyId, boxCollider.MotionType);
        }
    }

    namespace Simulate
    {
        void system(onyx::physics::PhysicsWorld& physicsSystem)
        {
            physicsSystem.Update();
        }
    }

    namespace PostPhysics
    {
        using Access = ecs::Access
            ::Read<components::PhysicsBodyId>
            ::Write<TransformComponent>
            ::With<DynamicBody>;

        using PhysiscsEntity = Access::AsEntity;

        void system(PhysiscsEntity entity, onyx::physics::PhysicsWorld& physicsSystem)
        {
            auto&& [physicsBody, transform] = entity;
            transform.Translation = physicsSystem.GetPosition(physicsBody.BodyId);
        }
    }

    namespace DebugDraw
    {
        using Access = ecs::Access
            ::Read<TransformComponent>
            ::ReadIfExists<SphereColliderComponent, BoxColliderComponent>
            ::With<components::PhysicsBodyId>;

        using ColliderEntity = Access::AsEntity;

        void system(ColliderEntity entity, onyx::graphics::DebugDrawQueue& debugDraw)
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

    void registerSystems(ecs::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterSystem(StreamIn::initBoxColliders);
        ecsBuilder.RegisterSystem(StreamIn::initSphereColliders);

        ecsBuilder.RegisterSystem(Simulate::system);
        ecsBuilder.RegisterSystem(PostPhysics::system);

        ecsBuilder.RegisterSystem(DebugDraw::system);

        ecsBuilder.RegisterComponent<BoxColliderComponent>(Init::factory);
        ecsBuilder.RegisterComponent<SphereColliderComponent>(Init::factory);
    }
}
