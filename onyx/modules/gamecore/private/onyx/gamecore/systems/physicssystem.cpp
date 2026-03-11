#include <onyx/gamecore/systems/physicssystem.h>

#include <onyx/graphics/debug/debugdrawqueue.h>
#include <onyx/entity/entitycomponentsystem.h>
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
        using EntityAccess = Entity::Entity<const TransformComponent, InitPhysics>;
        void system(EntityAccess entity, Onyx::Physics::PhysicsWorld& physicsSystem, Entity::EntityCommandBuffer entityCommandBuffer)
        {
            auto&& [transform] = entity.Get();

            Onyx::Physics::PhysicsBodyId bodyId;
            Onyx::Physics::MotionType motionType = Onyx::Physics::MotionType::Static;
            if( const BoxColliderComponent* boxCollider = entity.TryGetComponent<const BoxColliderComponent>() )
            {
                motionType = boxCollider->MotionType;
                bodyId = physicsSystem.CreateBoxCollider(transform.Translation, transform.Rotation, boxCollider->HalfExtents, boxCollider->MotionType, boxCollider->Layer);
            }
            else if( const SphereColliderComponent* sphereCollider = entity.TryGetComponent<const SphereColliderComponent>() )
            {
                motionType = sphereCollider->MotionType;
                bodyId = physicsSystem.CreateSphereCollider(transform.Translation, transform.Rotation, sphereCollider->Radius, sphereCollider->MotionType, sphereCollider->Layer);
            }

            entityCommandBuffer.AddComponent<Components::PhysicsBodyId>(entity, bodyId);
            if (motionType == Onyx::Physics::MotionType::Dynamic)
            {
                entityCommandBuffer.AddComponent<DynamicBody>(entity);
            }
            //else
            //{
            //    entityCommandBuffer.RemoveComponent<DynamicBody>(entity);
            //}

            entityCommandBuffer.RemoveComponent<InitPhysics>(entity);
        }
    }

    namespace Simulate
    {
        //using EntityAccess = Entity::Entity<const BoxColliderComponent, const TransformComponent, InitPhysics>;
        void system(Onyx::Physics::PhysicsWorld& physicsSystem)
        {
            physicsSystem.Update();
        }
    }

    namespace PostPhysics
    {
        using EntityAccess = Entity::Entity<const Components::PhysicsBodyId, TransformComponent, const DynamicBody>;
        void system(EntityAccess entity, Onyx::Physics::PhysicsWorld& physicsSystem)
        {
            auto&& [physicsBody, transform] = entity.Get();
            transform.Translation = physicsSystem.GetPosition(physicsBody.BodyId);
        }
    }

    namespace DebugDraw
    {
        using EntityAccess = Entity::Entity< const Components::PhysicsBodyId, const TransformComponent >;
        void system(EntityAccess entity, Onyx::Graphics::DebugDrawQueue debugDraw)
        {
            auto&& [_, transform] = entity.Get();
            if( const SphereColliderComponent* sphereCollider = entity.TryGetComponent<const SphereColliderComponent>() )
            {
                debugDraw.DrawSphere(transform.Translation, sphereCollider->Radius, 0xFF000077);
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
