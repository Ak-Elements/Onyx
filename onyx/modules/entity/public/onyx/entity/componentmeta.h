#pragma once

#include <onyx/entity/entityregistry.h>

namespace onyx::Editor {
template < typename T >
struct ComponentEditor {
    static bool DrawProperties( T& /*component*/, bool /*showHidden*/ ) { return false; }
};
} // namespace onyx::Editor

namespace onyx::ecs {
namespace details {
template < typename T >
concept IsTransient = requires( T obj ) { T::IsTransient; };

template < typename T >
concept IsCodeOnly = requires( T obj ) { T::IsCodeOnly; };

template < typename T >
concept IsFlagComponent = std::is_empty_v< T >;
} // namespace details

class EntityRegistry;

struct IComponentMeta {
    virtual ~IComponentMeta() = default;

    virtual void create( EntityRegistry& registry, EntityId entity ) const = 0;
    virtual void create( EntityRegistry& registry, EntityId entity, const Deserializer& deserializer ) const = 0;
    virtual void create( EntityRegistry& registry, EntityId entity, const std::any& component ) const = 0;

    virtual void copy( EntityRegistry& registry, EntityId entity, const void* componentPtr ) const = 0;
    virtual void copy( void* componentPtr, std::any& outCopy ) const = 0;

    ONYX_NO_DISCARD virtual bool hasFactory() const = 0;

    virtual bool serialize( const void* componentAny, Serializer& ) const = 0;
    virtual bool deserialize( void* componentAny, const Deserializer& ) const = 0;

#if !ONYX_IS_RETAIL
    // virtual bool DrawPropertyGridEditor(void* componentAny) const = 0;
#endif
    ONYX_NO_DISCARD virtual constexpr bool isTransient() const = 0;
    ONYX_NO_DISCARD virtual constexpr bool isCodeOnly() const = 0;
    ONYX_NO_DISCARD virtual constexpr bool isFlag() const = 0;
    ONYX_NO_DISCARD virtual constexpr StringId32 getTypeId() const = 0;
    ONYX_NO_DISCARD virtual constexpr uint32_t getRuntimeTypeId() const = 0;
};

template < typename T >
using ComponentFactoryFunction = void ( * )( EntityRegistry&, EntityId, T&& component );

template < typename T > requires std::copy_constructible< T >
struct ComponentMeta : public IComponentMeta {
  private:
    using DrawComponentPropertiesFunction = bool ( * )( bool );

  public:
    static_assert(
        details::IsTransient< T > || details::IsFlagComponent< T > || (Serializable< T > && Deserializable< T >),
        "Component needs to be either marked as transient or implement Serialize / Deserialize capabilities." );

    ComponentMeta() = default;
    ComponentMeta( ComponentFactoryFunction< T > factory )
        : m_factory( factory ) {}

    constexpr bool isTransient() const override { return details::IsTransient< T >; }
    constexpr bool isCodeOnly() const override { return details::IsCodeOnly< T >; }
    constexpr bool isFlag() const override { return details::IsFlagComponent< T >; }

    constexpr StringId32 getTypeId() const override {
        if constexpr( HasTypeId< T > ) {
            return T::TypeId;
        } else {
            ONYX_ASSERT( false, "Calling get typeid on a component that has no type id defined" );
            return {};
        }
    }

    constexpr uint32_t getRuntimeTypeId() const override { return TypeHash< T >(); }

    void create( EntityRegistry& registry, EntityId entity ) const override {
        if constexpr( details::IsFlagComponent< T > ) {
            registry.addComponent< T >( entity );
        } else if constexpr( Deserializable< T > ) {
            T component{};
            if( m_factory ) {
                m_factory( registry, entity, std::move( component ) );
            } else {
                registry.addComponent< T >( entity, component );
            }
        } else {
            ONYX_ASSERT( false, "Not supported for component" );
        }
    }

    template < typename... Args >
    void create( EntityRegistry& registry, EntityId entity, [[maybe_unused]] Args&&... args ) const {
        if constexpr( details::IsFlagComponent< T > ) {
            registry.addComponent< T >( entity );
        } else if constexpr( Deserializable< T > ) {
            T component( std::forward< Args >( args )... );
            if( m_factory ) {
                m_factory( registry, entity, std::move( component ) );
            } else {
                registry.addComponent< T >( entity, component );
            }
        } else {
            ONYX_ASSERT( false, "Not supported for component" );
        }
    }

    void create( EntityRegistry& registry, EntityId entity, const Deserializer& deserializer ) const override {
        if constexpr( details::IsFlagComponent< T > ) {
            registry.addComponent< T >( entity );
        } else if constexpr( Deserializable< T > ) {
            T component{};
            Serialization< T >::deserialize( deserializer, component );

            if( m_factory ) {
                m_factory( registry, entity, std::move( component ) );
            } else {
                registry.addComponent< T >( entity, component );
            }
        } else {
            ONYX_ASSERT( false, "Not supported for component" );
        }
    }

    void create( EntityRegistry& registry, EntityId entity, const std::any& component ) const override {
        const T& typedComponent = std::any_cast< const T >( component );
        create( registry, entity, typedComponent );
    }

    void copy( EntityRegistry& registry, EntityId entity, const void* componentPtr ) const override {
        if constexpr( details::IsFlagComponent< T > ) {
            registry.addComponent< T >( entity );
        } else {
            const T* component = static_cast< const T* >( componentPtr );
            if( m_factory ) {
                T copied = *component;
                m_factory( registry, entity, std::move( copied ) );
            } else {
                registry.addComponent< T >( entity, *component );
            }
        }
    }

    void copy( void* componentPtr, std::any& outCopy ) const override { outCopy = *static_cast< T* >( componentPtr ); }

    bool hasFactory() const override { return m_factory != nullptr; }

    bool serialize( const void* componentAny, Serializer& serializer ) const override {
        if constexpr( Serializable< T > ) {
            const T* component = static_cast< const T* >( componentAny );
            Serialization< T >::serialize( serializer, *component );
            return true;
        } else {
            ONYX_ASSERT( false, "Not supported for component" );
            return false;
        }
    }

    bool deserialize( void* componentAny, const Deserializer& deserializer ) const override {
        if constexpr( Deserializable< T > ) {
            T* component = static_cast< T* >( componentAny );
            Serialization< T >::deserialize( deserializer, *component );
            return true;
        } else {
            ONYX_ASSERT( false, "Not supported for component" );
            return false;
        }
    }

  private:
    StringId32 m_typeId;
    InplaceFunction< void( EntityRegistry&, EntityId, T&& ) > m_factory;
};
} // namespace onyx::ecs
