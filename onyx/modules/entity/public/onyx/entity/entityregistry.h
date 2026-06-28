#pragma once

#include <onyx/entity/dependantfunctionargument.h>
#include <onyx/entity/entity.h>

#include <entt/entity/registry.hpp>

namespace onyx::ecs {
class ComponentFactory;
struct ECSExecutionContext;

class EntityRegistry {
  public:
    using EntityRegistryT = entt::basic_registry< EntityId >;

#if ONYX_IS_EDITOR
    template < typename StorageIterableT >
    class ComponentViewT {
      public:
        using StorageIterable = StorageIterableT;
        using StorageIterator = decltype( std::declval< StorageIterable& >().begin() );

        class Iterator {
          public:
            Iterator( StorageIterator it, StorageIterator end, ecs::EntityId entity )
                : m_it( it )
                , m_end( end )
                , m_entity( entity ) {
                skipToValid();
            }

            Iterator& operator++() {
                ++m_it;
                skipToValid();
                return *this;
            }

            bool operator!=( const Iterator& other ) const { return m_it != other.m_it; }

            auto operator*() const { return *m_it; }

          private:
            void skipToValid() {
                while( m_it != m_end && !m_it->second.contains( m_entity ) ) {
                    ++m_it;
                }
            }

            StorageIterator m_it;
            StorageIterator m_end;
            ecs::EntityId m_entity;
        };

        ComponentViewT( StorageIterable storage, ecs::EntityId entity )
            : m_storage( std::move( storage ) )
            , m_entity( entity ) {}

        Iterator begin() { return Iterator( m_storage.begin(), m_storage.end(), m_entity ); }
        Iterator end() { return Iterator( m_storage.end(), m_storage.end(), m_entity ); }

      private:
        StorageIterable m_storage;
        ecs::EntityId m_entity;
    };

    using ComponentView = ComponentViewT< EntityRegistryT::iterable >;
    using ConstComponentView = ComponentViewT< EntityRegistryT::const_iterable >;
#endif

    // Both of those should be removed as we only need them because of the component factory dependency
    EntityRegistry() = default;
    EntityRegistry( ComponentFactory& componentFactory );

    EntityRegistry( const EntityRegistry& other );
    EntityRegistry( EntityRegistry&& other ) noexcept;

    EntityRegistry& operator=( const EntityRegistry& other );
    EntityRegistry& operator=( EntityRegistry&& other ) noexcept;

    // bool IsEmpty() const { return m_Registry.storage() }
    EntityId createEntity();
    EntityId createEntity( EntityId entity );
    void deleteEntity( EntityId entity );

    EntityId copyEntity( EntityId entity );

    template < typename T, typename... Args > requires( std::is_empty_v< T > )
    void addComponent( EntityId entity ) {
        ONYX_ASSERT( entity != EntityId::Invalid );
        m_registry.emplace_or_replace< T >( entity );
    }

    template < typename T, typename... Args > requires( std::is_empty_v< T > == false )
    T& addComponent( EntityId entity, Args&&... args ) {
        ONYX_ASSERT( entity != EntityId::Invalid );
        return m_registry.emplace_or_replace< T >( entity, std::forward< Args >( args )... );
    }

    template < typename T >
    void removeComponent( EntityId entity ) {
        ONYX_ASSERT( entity != EntityId::Invalid );
        m_registry.remove< T >( entity );
    }

    template < typename T >
    void removeIfExists( EntityId entity ) {
        if( hasComponent< T >( entity ) == false )
            return;

        m_registry.remove< T >( entity );
    }

    template < typename T >
    [[nodiscard]] bool hasComponent( EntityId entity ) const {
        ONYX_ASSERT( entity != EntityId::Invalid );
        return m_registry.all_of< T >( entity );
    }

#if ONYX_IS_EDITOR
    [[nodiscard]] bool hasComponent( EntityId entity, entt::id_type runtimeTypeId ) const {
        return m_registry.storage( runtimeTypeId )->value( entity ) != nullptr;
    }
#endif

    template < typename... Args >
    [[nodiscard]] bool hasComponents( EntityId entity ) const {
        ONYX_ASSERT( entity != EntityId::Invalid );
        return m_registry.all_of< Args... >( entity );
    }

    template < typename T >
    T& getComponent( EntityId entity ) {
        ONYX_ASSERT( entity != EntityId::Invalid );
        return m_registry.get< T >( entity );
    }

    template < typename T >
    const T& getComponent( EntityId entity ) const {
        ONYX_ASSERT( entity != EntityId::Invalid );
        return m_registry.get< T >( entity );
    }

    template < typename T >
    T* tryGetComponent( EntityId entity ) {
        ONYX_ASSERT( entity != EntityId::Invalid );
        return m_registry.try_get< T >( entity );
    }

    template < typename T >
    const T* tryGetComponent( EntityId entity ) const {
        ONYX_ASSERT( entity != EntityId::Invalid );
        return m_registry.try_get< T >( entity );
    }

    template < typename Type, typename... Other, typename... Excludes >
    [[nodiscard]] auto getView( Excludes&&... excludes ) {
        return m_registry.view< Type, Other... >( std::forward< Excludes >( excludes )... );
    }

    template < typename Type, typename... Other, typename... Excludes >
    [[nodiscard]] auto getView( Excludes&&... excludes ) const {
        return m_registry.view< Type, Other... >( std::forward< Excludes >( excludes )... );
    }

    template < typename... Owned, typename... Get, typename... Exclude >
    [[nodiscard]] auto getGroup( entt::get_t< Get... > observed = entt::get_t{},
                                 entt::exclude_t< Exclude... > excludes = entt::exclude_t{} ) {
        return m_registry.group< Owned... >( std::forward< entt::get_t< Get... > >( observed ),
                                             std::forward< entt::exclude_t< Exclude... > >( excludes ) );
    }

#if ONYX_IS_EDITOR
    [[nodiscard]] ComponentView getComponents( ecs::EntityId entity );
    [[nodiscard]] ConstComponentView getComponents( ecs::EntityId entity ) const;
#endif

    EntityRegistryT& getRegistry();
    [[nodiscard]] const EntityRegistryT& getRegistry() const;

    void copy( EntityRegistry& toRegistry ) const;

    void clear();

  private:
    bool hasEntity( EntityId id ) { return m_registry.valid( id ); }

    [[nodiscard]] EntityRegistryT::iterable getStorage();
    [[nodiscard]] EntityRegistryT::const_iterable getStorage() const;

  private:
    EntityRegistryT m_registry;
    ComponentFactory* m_componentFactory = nullptr;
};

template <>
class DependantFunctionArg< EntityRegistry > {
  public:
    static const EntityRegistry& get( const ECSExecutionContext& context );
};
} // namespace onyx::ecs
