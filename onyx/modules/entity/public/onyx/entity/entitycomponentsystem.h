#pragma once

#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/ecsexecutioncontext.h>
#include <onyx/entity/entityaccess.h>
#include <onyx/entity/entityregistry.h>

namespace onyx::ecs {
class EntityRegistry;
}

namespace onyx::graphics {
struct FrameContext;
}

namespace onyx::ecs {
template <>
class DependantFunctionArg< DeltaGameTime > {
  public:
    static DeltaGameTime get( const ECSExecutionContext& context ) { return context.DeltaTime; }
};

class EntityComponentSystemsGraph {
  public:
    EntityComponentSystemsGraph() = default;
    ~EntityComponentSystemsGraph() = default;

    void setRegistry( EntityRegistry* reg ) { m_entityRegistry = reg; }

    template < typename Callable >
    void registerSystem( Callable callable ) {
        InplaceFunction< void( const ECSExecutionContext& ), 64 > functor = buildSystemCall< Callable >( callable );
        m_systems.emplace_back( functor );
    }

    void update( const ECSExecutionContext& context ) const;

  private:
    template < typename System, typename... EntityQueryArgs, typename... Args >
    static auto buildSystemCall( void ( *callable )( EntityQuery< EntityQueryArgs... >, Args... ) ) {
        return [ = ]( const ECSExecutionContext& context ) {
            InvokeWithTypeList( TypeList< Args... >{}, [ & ]< typename... FunctionArg >() {
                callable( EntityQuery< EntityQueryArgs... >( context.Registry ),
                          DependantFunctionArg< std::remove_cvref_t< FunctionArg > >::get( context )... );
            } );
        };
    }

    template < typename System, typename... EntityAccessT, typename... Args >
    static auto buildSystemCall( void ( *callable )( Entity< EntityAccessT... >, Args... ) ) {
        return [ = ]( const ECSExecutionContext& context ) {
            auto args = std::tuple< Args... >( DependantFunctionArg< std::remove_cvref_t< Args > >::get( context )... );

            EntityQuery< EntityAccessT... > query( context.Registry );
            const auto& entitiesView = query.getView();
            for( const EntityId entityId : entitiesView ) {
                Entity< EntityAccessT... > entity{ query, entityId };
                std::apply( [ & ]( auto&... a ) { callable( entity, a... ); }, args );
            }
        };
    }

    template < typename System, typename... Args >
    static auto buildSystemCall( void ( *callable )( Args... ) ) {
        return [ = ]( const ECSExecutionContext& context ) {
            InvokeWithTypeList( TypeList< Args... >{}, [ & ]< typename... FunctionArg >() {
                callable( DependantFunctionArg< std::remove_cvref_t< FunctionArg > >::get( context )... );
            } );
        };
    }

  private:
    EntityRegistry* m_entityRegistry = nullptr;
    // TODO: DAG?
    DynamicArray< InplaceFunction< void( const ECSExecutionContext& ), 64 > > m_systems;
};
} // namespace onyx::ecs
