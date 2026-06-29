#pragma once

#include <onyx/entity/componentmeta.h>

#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>

namespace onyx::ecs {
class ComponentFactory {
  public:
    template < typename T >
    void registerComponent() {
        if( const IComponentMeta* componentMeta = getComponentMeta< T >().value_or( nullptr ) ) {
            // TODO add some sanity check that the components registered are indeed the same
            std::ignore = componentMeta;
            return;
        }

        m_componentMeta[ T::TypeId ] = makeUnique< ComponentMeta< T > >();
        m_runtimeIdToStaticId[ entt::type_hash< T >::value() ] = T::TypeId;
    }

    template < typename T >
    void registerComponent( ComponentFactoryFunction< T > factory ) {
        if( const IComponentMeta* componentMeta = getComponentMeta< T >().value_or( nullptr ) ) {
            // TODO add some sanity check that the components registered are indeed the same
            std::ignore = componentMeta;
            return;
        }

        m_componentMeta[ T::TypeId ] = makeUnique< ComponentMeta< T > >( factory );
        m_runtimeIdToStaticId[ entt::type_hash< T >::value() ] = T::TypeId;
    }

    template < typename T >
    Optional< const IComponentMeta* > getComponentMeta() const {
        auto it = m_componentMeta.find( T::TypeId );
        if( it == m_componentMeta.end() ) {
            return std::nullopt;
        }

        return it->second.get();
    }

    Optional< const IComponentMeta* > getComponentMeta( StringId32 typeId ) const {
        auto it = m_componentMeta.find( typeId );
        if( it == m_componentMeta.end() ) {
            return std::nullopt;
        }

        return it->second.get();
    }

    Optional< const IComponentMeta* > getComponentMeta( entt::id_type runtimeTypeId ) const {
        auto staticIdIt = m_runtimeIdToStaticId.find( runtimeTypeId );
        if( staticIdIt == m_runtimeIdToStaticId.end() ) {
            return std::nullopt;
        }

        return getComponentMeta( staticIdIt->second );
    }

    const HashMap< StringId32, UniquePtr< IComponentMeta > >& getComponentMeta() const { return m_componentMeta; }

    template < typename T >
    bool tryCreateComponent( EntityRegistry& registry, EntityId entityId ) const {
        return tryCreateComponent( registry, entityId, T::TypeId );
    }

    template < typename T, typename... Args >
    bool tryCreateComponent( EntityRegistry& registry, EntityId entityId, Args&&... args ) const {
        const ComponentMeta< T >* componentMeta = static_cast< const ComponentMeta< T >* >(
            getComponentMeta( T::TypeId ).value_or( nullptr ) );
        if( componentMeta != nullptr ) {
            componentMeta->create( registry, entityId, std::forward< Args >( args )... );
            return true;
        }

        return false;
    }

    bool tryCreateComponent( EntityRegistry& registry, EntityId entityId, StringId32 componentTypeId ) const;
    bool tryCreateComponent( EntityRegistry& registry,
                             EntityId entityId,
                             StringId32 componentTypeId,
                             const Deserializer& deserializer ) const;

    bool tryCopyComponent( EntityRegistry& registry,
                           EntityId entityId,
                           StringId32 componentTypeId,
                           void* fromComponentPtr ) const;

    bool tryCreateComponent( EntityRegistry& registry,
                             EntityId entityId,
                             StringId32 componentTypeId,
                             const std::any& component ) const;

  private:
    HashMap< StringId32, UniquePtr< IComponentMeta > > m_componentMeta;
    HashMap< entt::id_type, StringId32 > m_runtimeIdToStaticId;
};
} // namespace onyx::ecs
