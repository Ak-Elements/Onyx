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
    static DeltaGameTime Get( const ECSExecutionContext& context ) { return context.DeltaTime; }
};

class EntityComponentSystemsGraph {
  public:
    EntityComponentSystemsGraph() = default;
    ~EntityComponentSystemsGraph() = default;

    void SetRegistry( EntityRegistry* reg ) { m_EntityRegistry = reg; }

    template < typename Callable >
    void Register( Callable callable ) {
        InplaceFunction< void( const ECSExecutionContext& ), 64 > functor = BuildSystemCall< Callable >( callable );
        m_Systems.emplace_back( functor );
    }

    void Update( const ECSExecutionContext& context ) const;

  private:
    template < typename System, typename... EntityQueryArgs, typename... Args >
    static auto BuildSystemCall( void ( *callable )( EntityQuery< EntityQueryArgs... >, Args... ) ) {
        return [ = ]( const ECSExecutionContext& context ) {
            InvokeWithTypeList( TypeList< Args... >{}, [ & ]< typename... FunctionArg >() {
                callable( EntityQuery< EntityQueryArgs... >( context.Registry ),
                          DependantFunctionArg< std::remove_cvref_t< FunctionArg > >::Get( context )... );
            } );
        };
    }

    template < typename System, typename... EntityAccessT, typename... Args >
    static auto BuildSystemCall( void ( *callable )( Entity< EntityAccessT... >, Args... ) ) {
        return [ = ]( const ECSExecutionContext& context ) {
            auto args = std::tuple< Args... >( DependantFunctionArg< std::remove_cvref_t< Args > >::Get( context )... );

            EntityQuery< EntityAccessT... > query( context.Registry );
            const auto& entitiesView = query.getView();
            for( const EntityId entityId : entitiesView ) {
                Entity< EntityAccessT... > entity{ query, entityId };
                std::apply( [ & ]( auto&... a ) { callable( entity, a... ); }, args );
            }
        };
    }

    template < typename System, typename... Args >
    static auto BuildSystemCall( void ( *callable )( Args... ) ) {
        return [ = ]( const ECSExecutionContext& context ) {
            InvokeWithTypeList( TypeList< Args... >{}, [ & ]< typename... FunctionArg >() {
                callable( DependantFunctionArg< std::remove_cvref_t< FunctionArg > >::Get( context )... );
            } );
        };
    }

  private:
    EntityRegistry* m_EntityRegistry;
    // TODO: DAG?
    DynamicArray< InplaceFunction< void( const ECSExecutionContext& ), 64 > > m_Systems;
};
} // namespace onyx::ecs
