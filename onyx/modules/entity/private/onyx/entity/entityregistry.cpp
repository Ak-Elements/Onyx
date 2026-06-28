#include <onyx/entity/entityregistry.h>

#include <onyx/entity/componentfactory.h>
#include <onyx/entity/ecsexecutioncontext.h>

namespace onyx::ecs {

EntityRegistry::EntityRegistry( ComponentFactory& factory )
    : m_componentFactory( &factory ) {}

EntityRegistry::EntityRegistry( const EntityRegistry& other )
    : m_componentFactory( other.m_componentFactory ) {
    m_registry.clear();
    other.copy( *this );
}

EntityRegistry::EntityRegistry( EntityRegistry&& other ) noexcept
    : m_registry( std::move( other.m_registry ) )
    , m_componentFactory( other.m_componentFactory ) {}

EntityRegistry& EntityRegistry::operator=( const EntityRegistry& other ) {
    if( this == &other )
        return *this;

    m_registry.clear();
    m_componentFactory = other.m_componentFactory;
    other.copy( *this );

    return *this;
}

EntityRegistry& EntityRegistry::operator=( EntityRegistry&& other ) noexcept {
    if( this == &other )
        return *this;

    other.m_registry.swap( m_registry );
    m_componentFactory = other.m_componentFactory;

    return *this;
}

EntityId EntityRegistry::createEntity() {
    return m_registry.create();
}

EntityId EntityRegistry::createEntity( EntityId entity ) {
    ONYX_ASSERT( entity != EntityId::Invalid );
    return m_registry.create( entity );
}

void EntityRegistry::deleteEntity( EntityId entity ) {
    ONYX_ASSERT( entity != EntityId::Invalid );
    m_registry.destroy( entity );
}

EntityId EntityRegistry::copyEntity( EntityId entity ) {
    ONYX_ASSERT( entity != EntityId::Invalid );
    EntityId newEntity = createEntity();

    // create a copy of an entity component by component
    for( const auto&& componentStorageIt : getStorage() ) {
        if( auto& componentStorage = componentStorageIt.second; componentStorage.contains( entity ) ) {
            componentStorage.push( newEntity, componentStorage.value( entity ) );
        }
    }

    return newEntity;
}

EntityRegistry::ComponentView EntityRegistry::getComponents( ecs::EntityId entity ) {
    return { m_registry.storage(), entity };
}

EntityRegistry::ConstComponentView EntityRegistry::getComponents( ecs::EntityId entity ) const {
    return { m_registry.storage(), entity };
}

EntityRegistry::EntityRegistryT::iterable EntityRegistry::getStorage() {
    return m_registry.storage();
}

EntityRegistry::EntityRegistryT::const_iterable EntityRegistry::getStorage() const {
    return m_registry.storage();
}

EntityRegistry::EntityRegistryT& EntityRegistry::getRegistry() {
    return m_registry;
}

const EntityRegistry::EntityRegistryT& EntityRegistry::getRegistry() const {
    return m_registry;
}

void EntityRegistry::copy( EntityRegistry& toRegistry ) const {
    ONYX_ASSERT( m_componentFactory != nullptr );

    // ONYX_ASSERT(toRegistry.IsEmpty(), "Registry to copy to has to be empty.");
    //  create a copy of an entity component by component
    for( const auto&& componentStorageIt : getStorage() ) {
        const IComponentMeta* componentMeta = m_componentFactory->GetComponentMeta( componentStorageIt.first )
                                                  .value_or( nullptr );
        if( componentMeta == nullptr )
            continue;

        for( EntityId entityId : componentStorageIt.second ) {
            if( toRegistry.hasEntity( entityId ) == false ) {
                [[maybe_unused]] EntityId newEntityId = toRegistry.createEntity( entityId );
                ONYX_ASSERT( newEntityId == entityId );
            }

            const void* fromComponentPtr = componentStorageIt.second.value( entityId );
            componentMeta->copy( toRegistry, entityId, fromComponentPtr );
        }
    }
}

void EntityRegistry::clear() {
    m_registry.clear();
}

const EntityRegistry& DependantFunctionArg< EntityRegistry >::get( const ECSExecutionContext& context ) {
    return context.Registry;
}
} // namespace onyx::ecs
