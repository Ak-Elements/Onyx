#pragma once

#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/ecsexecutioncontext.h>
#include <onyx/entity/entityregistry.h>

namespace onyx::ecs {
template < typename... ComponentAccessDefinition >
struct EntityQuery;

template < typename... ComponentAccessDefinition >
struct Entity;

struct Access {
    enum class MemoryAccess : uint8_t { None, Read, Write };

    enum class Filter : uint8_t { Include, Exclude, Maybe };

  private:
    template < typename T, MemoryAccess A, Filter F >
    struct ComponentAccessDefinition {
        using Type = T;
        static constexpr MemoryAccess Access = A;
        static constexpr Filter Filter = F;
    };

    template < typename... ComponentAccessDefinitions >
    struct AccessBuilder {
        template < typename... Ts >
        using Read = AccessBuilder< ComponentAccessDefinitions...,
                                    ComponentAccessDefinition< Ts, MemoryAccess::Read, Filter::Include >... >;

        template < typename... Ts >
        using ReadIfExists = AccessBuilder< ComponentAccessDefinitions...,
                                            ComponentAccessDefinition< Ts, MemoryAccess::Read, Filter::Maybe >... >;

        template < typename... Ts >
        using Write = AccessBuilder< ComponentAccessDefinitions...,
                                     ComponentAccessDefinition< Ts, MemoryAccess::Write, Filter::Include >... >;

        template < typename... Ts >
        using WriteIfExits = AccessBuilder< ComponentAccessDefinitions...,
                                            ComponentAccessDefinition< Ts, MemoryAccess::Write, Filter::Maybe >... >;

        template < typename... Ts >
        using With = AccessBuilder< ComponentAccessDefinitions...,
                                    ComponentAccessDefinition< Ts, MemoryAccess::None, Filter::Include >... >;

        template < typename... Ts >
        using Without = AccessBuilder< ComponentAccessDefinitions...,
                                       ComponentAccessDefinition< Ts, MemoryAccess::None, Filter::Exclude >... >;

        template < typename... Ts >
        using Has = AccessBuilder< ComponentAccessDefinitions...,
                                   ComponentAccessDefinition< Ts, MemoryAccess::None, Filter::Maybe >... >;

        using ReadComponents = decltype( std::tuple_cat(
            std::conditional_t< ( ComponentAccessDefinitions::Access == MemoryAccess::Read ),
                                std::tuple< typename ComponentAccessDefinitions::Type >,
                                std::tuple<> >{}... ) );

        using WriteComponents = decltype( std::tuple_cat(
            std::conditional_t< ( ComponentAccessDefinitions::Access == MemoryAccess::Write ),
                                std::tuple< typename ComponentAccessDefinitions::Type >,
                                std::tuple<> >{}... ) );

        using AsQuery = EntityQuery< ComponentAccessDefinitions... >;
        using AsEntity = Entity< ComponentAccessDefinitions... >;
    };

  public:
    template < typename... Ts >
    using Read = AccessBuilder< ComponentAccessDefinition< Ts, MemoryAccess::Read, Filter::Include >... >;

    template < typename... Ts >
    using ReadIfExists = AccessBuilder< ComponentAccessDefinition< Ts, MemoryAccess::Read, Filter::Maybe >... >;

    template < typename... Ts >
    using Write = AccessBuilder< ComponentAccessDefinition< Ts, MemoryAccess::Write, Filter::Include >... >;

    template < typename... Ts >
    using WriteIfExits = AccessBuilder< ComponentAccessDefinition< Ts, MemoryAccess::Write, Filter::Maybe >... >;

    template < typename... Ts >
    using With = AccessBuilder< ComponentAccessDefinition< Ts, MemoryAccess::None, Filter::Include >... >;

    template < typename... Ts >
    using Without = AccessBuilder< ComponentAccessDefinition< Ts, MemoryAccess::None, Filter::Exclude >... >;

    template < typename... Ts >
    using Has = AccessBuilder< ComponentAccessDefinition< Ts, MemoryAccess::None, Filter::Maybe >... >;
};

template < typename... ComponentAccessDefinitions >
struct EntityQuery {
  private:
    template < typename... Included, typename... Excluded >
    static consteval auto makeViewType( std::tuple< Included... >, std::tuple< Excluded... > ) {
        return entt::basic_view<
            entt::get_t< EntityRegistry::EntityRegistryT::storage_for_type<
                std::conditional_t< Included::Access == Access::MemoryAccess::Write,
                                    typename Included::Type,
                                    const typename Included::Type > >... >,
            entt::exclude_t< EntityRegistry::EntityRegistryT::storage_for_type< typename Excluded::Type >... > >();
    }

  public:
    using IncludedComponents = decltype( std::tuple_cat(
        std::conditional_t< ( ComponentAccessDefinitions::Filter == Access::Filter::Include ),
                            std::tuple< ComponentAccessDefinitions >,
                            std::tuple<> >{}... ) );

    using ExcludedComponents = decltype( std::tuple_cat(
        std::conditional_t< ( ComponentAccessDefinitions::Filter == Access::Filter::Exclude ),
                            std::tuple< ComponentAccessDefinitions >,
                            std::tuple<> >{}... ) );

    using ViewT = decltype( makeViewType( IncludedComponents{}, ExcludedComponents{} ) );

    EntityQuery( const EntityQuery& ) = delete;
    EntityQuery& operator=( const EntityQuery& ) = delete;

    EntityQuery( EntityRegistry& registry )
        : m_view( makeView( registry, IncludedComponents{}, ExcludedComponents{} ) )
        , m_entityRegistry( &registry ) {}

    ViewT& getView() { return m_view; }
    const ViewT& getView() const { return m_view; }

    EntityRegistry* getRegistry() { return m_entityRegistry; }

    ViewT::iterator begin() { return m_view.begin(); }
    ViewT::iterator end() { return m_view.end(); }

  private:
    template < typename... Included, typename... Excluded >
    static ViewT makeView( EntityRegistry& registry, std::tuple< Included... >, std::tuple< Excluded... > ) {
        return registry.getView< std::conditional_t< Included::Access == Access::MemoryAccess::Write, // Condition
                                                     typename Included::Type,
                                                     const typename Included::Type >... > // Type
            ( entt::exclude_t< typename Excluded::Type... >{} );
    }

  private:
    ViewT m_view;
    EntityRegistry* m_entityRegistry = nullptr;
};

template < typename... ComponentAccessDefinitions >
struct Entity {
  public:
    using IncludedMemoryAccessComponents = decltype( std::tuple_cat(
        std::conditional_t < ( ComponentAccessDefinitions::Access != Access::MemoryAccess::None ) && // Condition
            ( ComponentAccessDefinitions::Filter == Access::Filter::Include ),                       // Condition
        std::tuple< ComponentAccessDefinitions >,
        std::tuple < >> {}... ) );

    Entity( EntityQuery< ComponentAccessDefinitions... >& query, EntityId entityId )
        : m_query( query )
        , m_entityId( entityId ) {}

    template < std::size_t I >
    decltype( auto ) get() {
        using ComponentAccessDefinition = std::tuple_element_t< I, IncludedMemoryAccessComponents >;
        using ComponentT = std::conditional_t< ComponentAccessDefinition::Access == Access::MemoryAccess::Write,
                                               typename ComponentAccessDefinition::Type,
                                               const typename ComponentAccessDefinition::Type >;

        return m_query.getView().template get< ComponentT >( m_entityId );
    }

    [[nodiscard]] EntityId getId() const;

    template < typename T > requires( ( std::is_same_v< T, typename ComponentAccessDefinitions::Type > &&
                                        ComponentAccessDefinitions::Filter == Access::Filter::Maybe &&
                                        ComponentAccessDefinitions::Access == Access::MemoryAccess::Write ) ||
                                      ... )
    T* tryGetComponent() {
        return m_query.getRegistry()->template tryGetComponent< T >( m_entityId );
    }

    template < typename T > requires( ( std::is_same_v< T, typename ComponentAccessDefinitions::Type > &&
                                        ComponentAccessDefinitions::Filter == Access::Filter::Maybe &&
                                        ComponentAccessDefinitions::Access == Access::MemoryAccess::Read ) ||
                                      ... )
    const T* tryGetComponent() const {
        return m_query.getRegistry()->template tryGetComponent< T >( m_entityId );
    }

    template < typename T > requires( ( std::is_same_v< T, typename ComponentAccessDefinitions::Type > &&
                                        ComponentAccessDefinitions::Filter == Access::Filter::Maybe &&
                                        ComponentAccessDefinitions::Access == Access::MemoryAccess::None ) ||
                                      ... )
    [[nodiscard]] bool hasComponent() const {
        return m_query.getRegistry()->template hasComponent< T >( m_entityId );
    }

  private:
    EntityQuery< ComponentAccessDefinitions... >& m_query;
    EntityId m_entityId;
};
template < typename... ComponentAccessDefinitions >
inline EntityId Entity< ComponentAccessDefinitions... >::getId() const {
    return m_entityId;
}

template < typename... ComponentAccessDefinitions >
class DependantFunctionArg< EntityQuery< ComponentAccessDefinitions... > > {
  public:
    static EntityQuery< ComponentAccessDefinitions... > Get( const ECSExecutionContext& context ) {
        return EntityQuery< ComponentAccessDefinitions... >( context.Registry );
    }
};
} // namespace onyx::ecs

// NOLINTBEGIN
namespace std {
template < typename... ComponentAccessDefintions >
struct tuple_size< onyx::ecs::Entity< ComponentAccessDefintions... > >
    : std::integral_constant< std::size_t,
                              std::tuple_size_v< typename onyx::ecs::Entity<
                                  ComponentAccessDefintions... >::IncludedMemoryAccessComponents > > {};

template < std::size_t I, typename... ComponentAccessDefintions >
struct tuple_element< I, onyx::ecs::Entity< ComponentAccessDefintions... > > {
    using IncludedMemoryAccessComponents = typename onyx::ecs::Entity<
        ComponentAccessDefintions... >::IncludedMemoryAccessComponents;
    using ComponentAccessDefinition = std::tuple_element_t< I, IncludedMemoryAccessComponents >;

    using type = std::conditional_t< ComponentAccessDefinition::Access == onyx::ecs::Access::MemoryAccess::Write,
                                     typename ComponentAccessDefinition::Type,
                                     const typename ComponentAccessDefinition::Type >;
};
// NOLINTEND

} // namespace std
