#pragma once

#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/ecsexecutioncontext.h>
#include <onyx/entity/entityaccess.h>
#include <onyx/entity/entityregistry.h>

namespace onyx::ecs {
struct EntityCommandBuffer {
    explicit EntityCommandBuffer( EntityRegistry& registry )
        : m_registry( &registry ) {}

    // TODO: they should probably be run at the end of the frame not at the end of the system call
    ~EntityCommandBuffer() {
        for( const auto& queuedCommand : m_queuedCommands ) {
            queuedCommand();
        }
    }

    template < typename T >
    void addComponent( EntityId entityId ) {
        m_queuedCommands.push_back(
            [ registry = m_registry, entityId ]() { registry->addComponent< T >( entityId ); } );
    }

    template < typename T, typename... Args >
    void addComponent( EntityId entityId, Args&&... args ) {
        m_queuedCommands.push_back(
            [ registry = m_registry, entityId, ... args = std::forward< Args >( args ) ]() mutable {
                registry->addComponent< T >( entityId, std::forward< Args >( args )... );
            } );
    }

    template < typename T, typename EntityAccessT, typename... Args >
    requires is_specialization_of_v< Entity, EntityAccessT >
    void addComponent( EntityAccessT entity, Args&&... args ) {
        m_queuedCommands.push_back(
            [ registry = m_registry, entityId = entity.getId(), ... args = std::forward< Args >( args ) ]() mutable {
                registry->addComponent< T >( entityId, std::forward< Args >( args )... );
            } );
    }

    template < typename T >
    void removeComponent( EntityId entityId ) {
        m_queuedCommands.push_back(
            [ registry = m_registry, entityId ]() { registry->removeComponent< T >( entityId ); } );
    }

    template < typename T, typename EntityAccessT > requires is_specialization_of_v< Entity, EntityAccessT >
    void removeComponent( EntityAccessT entity ) {
        m_queuedCommands.push_back(
            [ registry = m_registry, entityId = entity.getId() ]() { registry->removeComponent< T >( entityId ); } );
    }

  private:
    EntityRegistry* m_registry = nullptr;
    DynamicArray< InplaceFunction< void() > > m_queuedCommands;
};

template <>
class DependantFunctionArg< EntityCommandBuffer > {
  public:
    static EntityCommandBuffer Get( const ECSExecutionContext& context ) {
        return EntityCommandBuffer( context.Registry );
    }
};
} // namespace onyx::ecs
