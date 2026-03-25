#pragma once

#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/ecsexecutioncontext.h>
#include <onyx/entity/entityaccess.h>
#include <onyx/entity/entityregistry.h>

namespace onyx::ecs {
struct EntityCommandBuffer {
    explicit EntityCommandBuffer( EntityRegistry& registry )
        : m_Registry( &registry ) {}

    // TODO: they should probably be run at the end of the frame not at the end of the system call
    ~EntityCommandBuffer() {
        for ( const auto& queuedCommand : m_QueuedCommands ) {
            queuedCommand();
        }
    }

    template < typename T >
    void AddComponent( EntityId entityId ) {
        m_QueuedCommands.push_back(
            [ registry = m_Registry, entityId ]() { registry->AddComponent< T >( entityId ); } );
    }

    template < typename T, typename... Args >
    void AddComponent( EntityId entityId, Args&&... args ) {
        m_QueuedCommands.push_back(
            [ registry = m_Registry, entityId, ... args = std::forward< Args >( args ) ]() mutable {
                registry->AddComponent< T >( entityId, std::forward< Args >( args )... );
            } );
    }

    template < typename T, typename EntityAccessT, typename... Args >
    requires is_specialization_of_v< Entity, EntityAccessT >
    void AddComponent( EntityAccessT entity, Args&&... args ) {
        m_QueuedCommands.push_back(
            [ registry = m_Registry, entityId = entity.GetId(), ... args = std::forward< Args >( args ) ]() mutable {
                registry->AddComponent< T >( entityId, std::forward< Args >( args )... );
            } );
    }

    template < typename T >
    void RemoveComponent( EntityId entityId ) {
        m_QueuedCommands.push_back(
            [ registry = m_Registry, entityId ]() { registry->RemoveComponent< T >( entityId ); } );
    }

    template < typename T, typename EntityAccessT > requires is_specialization_of_v< Entity, EntityAccessT >
    void RemoveComponent( EntityAccessT entity ) {
        m_QueuedCommands.push_back(
            [ registry = m_Registry, entityId = entity.GetId() ]() { registry->RemoveComponent< T >( entityId ); } );
    }

  private:
    EntityRegistry* m_Registry = nullptr;
    DynamicArray< InplaceFunction< void() > > m_QueuedCommands;
};

template <>
class DependantFunctionArg< EntityCommandBuffer > {
  public:
    static EntityCommandBuffer Get( const ECSExecutionContext& context ) {
        return EntityCommandBuffer( context.Registry );
    }
};
} // namespace onyx::ecs