#include <onyx/entity/componentfactory.h>

namespace onyx::ecs {
bool ComponentFactory::tryCreateComponent( EntityRegistry& registry,
                                           EntityId entity,
                                           StringId32 componentTypeId ) const {
    Optional< const IComponentMeta* > componentMetaOptional = getComponentMeta( componentTypeId );
    const IComponentMeta* componentMeta = *componentMetaOptional;
    componentMeta->create( registry, entity );
    return true;
}

bool ComponentFactory::tryCreateComponent( EntityRegistry& registry,
                                           EntityId entity,
                                           StringId32 componentTypeId,
                                           const Deserializer& deserializer ) const {
    const IComponentMeta* componentMeta = getComponentMeta( componentTypeId ).value_or( nullptr );
    if( componentMeta != nullptr ) {
        componentMeta->create( registry, entity, deserializer );
        return true;
    }

    return false;
}

bool ComponentFactory::tryCopyComponent( EntityRegistry& registry,
                                         EntityId entityId,
                                         StringId32 componentTypeId,
                                         void* fromComponentPtr ) const {
    const IComponentMeta* componentMeta = getComponentMeta( componentTypeId ).value_or( nullptr );
    if( componentMeta != nullptr ) {
        componentMeta->copy( registry, entityId, fromComponentPtr );
        return true;
    }

    return false;
}

bool ComponentFactory::tryCreateComponent( EntityRegistry& registry,
                                           EntityId entityId,
                                           StringId32 componentTypeId,
                                           const std::any& component ) const {
    const IComponentMeta* componentMeta = getComponentMeta( componentTypeId ).value_or( nullptr );
    if( componentMeta != nullptr ) {
        componentMeta->create( registry, entityId, component );
        return true;
    }

    return false;
}

} // namespace onyx::ecs
