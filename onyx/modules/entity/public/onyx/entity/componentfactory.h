#pragma once

#include <onyx/entity/componentmeta.h>

#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>

namespace onyx::ecs {
class ComponentFactory {
  public:
    template < typename T >
    void Register() {
        if ( const IComponentMeta* componentMeta = GetComponentMeta< T >().value_or( nullptr ) ) {
            // TODO add some sanity check that the components registered are indeed the same
            ONYX_UNUSED( componentMeta );
            return;
        }

        m_ComponentMeta[ T::TypeId ] = makeUnique< ComponentMeta< T > >();
        m_RuntimeIdToStaticId[ entt::type_hash< T >::value() ] = T::TypeId;
    }

    template < typename T >
    void Register( ComponentFactoryFunction< T > factory ) {
        if ( const IComponentMeta* componentMeta = GetComponentMeta< T >().value_or( nullptr ) ) {
            // TODO add some sanity check that the components registered are indeed the same
            ONYX_UNUSED( componentMeta );
            return;
        }

        m_ComponentMeta[ T::TypeId ] = makeUnique< ComponentMeta< T > >( factory );
        m_RuntimeIdToStaticId[ entt::type_hash< T >::value() ] = T::TypeId;
    }

    template < typename T >
    Optional< const IComponentMeta* > GetComponentMeta() const {
        auto it = m_ComponentMeta.find( T::TypeId );
        if ( it == m_ComponentMeta.end() ) {
            return std::nullopt;
        }

        return it->second.get();
    }

    Optional< const IComponentMeta* > GetComponentMeta( StringId32 typeId ) const {
        auto it = m_ComponentMeta.find( typeId );
        if ( it == m_ComponentMeta.end() ) {
            return std::nullopt;
        }

        return it->second.get();
    }

    Optional< const IComponentMeta* > GetComponentMeta( entt::id_type runtimeTypeId ) const {
        auto staticIdIt = m_RuntimeIdToStaticId.find( runtimeTypeId );
        if ( staticIdIt == m_RuntimeIdToStaticId.end() ) {
            return std::nullopt;
        }

        return GetComponentMeta( staticIdIt->second );
    }

    const HashMap< StringId32, UniquePtr< IComponentMeta > >& GetComponentMeta() const { return m_ComponentMeta; }

    template < typename T >
    bool TryCreateComponent( EntityRegistry& registry, EntityId entityId ) const {
        return TryCreateComponent( registry, entityId, T::TypeId );
    }

    template < typename T, typename... Args >
    bool TryCreateComponent( EntityRegistry& registry, EntityId entityId, Args&&... args ) const {
        const ComponentMeta< T >* componentMeta = static_cast< const ComponentMeta< T >* >(
            GetComponentMeta( T::TypeId ).value_or( nullptr ) );
        if ( componentMeta != nullptr ) {
            componentMeta->Create( registry, entityId, std::forward< Args >( args )... );
            return true;
        }

        return false;
    }

    bool TryCreateComponent( EntityRegistry& registry, EntityId entityId, StringId32 componentTypeId ) const;
    bool TryCreateComponent( EntityRegistry& registry,
                             EntityId entityId,
                             StringId32 componentTypeId,
                             const Deserializer& deserializer ) const;
    bool TryCreateComponent( EntityRegistry& registry,
                             EntityId entityId,
                             StringId32 componentTypeId,
                             Span< uint32_t > componentData ) const;

    bool TryCopyComponent( EntityRegistry& registry,
                           EntityId entityId,
                           StringId32 componentTypeId,
                           void* fromComponentPtr ) const;

  private:
    HashMap< StringId32, UniquePtr< IComponentMeta > > m_ComponentMeta;
    HashMap< entt::id_type, StringId32 > m_RuntimeIdToStaticId;
};
} // namespace onyx::ecs
