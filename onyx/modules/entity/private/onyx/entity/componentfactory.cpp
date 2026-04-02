#include <onyx/entity/componentfactory.h>

namespace onyx::ecs {
bool ComponentFactory::TryCreateComponent( EntityRegistry& registry, EntityId entity, StringId32 componentId ) const {
    Optional< const IComponentMeta* > componentMetaOptional = GetComponentMeta( componentId );
    const IComponentMeta* componentMeta = *componentMetaOptional;
    componentMeta->create( registry, entity );
    return true;
}

bool ComponentFactory::TryCreateComponent( EntityRegistry& registry,
                                           EntityId entity,
                                           StringId32 componentTypeId,
                                           const Deserializer& deserializer ) const {
    const IComponentMeta* componentMeta = GetComponentMeta( componentTypeId ).value_or( nullptr );
    if( componentMeta != nullptr ) {
        componentMeta->create( registry, entity, deserializer );
        return true;
    }

    return false;
}

bool ComponentFactory::TryCopyComponent( EntityRegistry& registry,
                                         EntityId entityId,
                                         StringId32 componentTypeId,
                                         void* fromComponentPtr ) const {
    const IComponentMeta* componentMeta = GetComponentMeta( componentTypeId ).value_or( nullptr );
    if( componentMeta != nullptr ) {
        componentMeta->copy( registry, entityId, fromComponentPtr );
        return true;
    }

    return false;
}

bool ComponentFactory::TryCreateComponent( EntityRegistry& registry,
                                           EntityId entityId,
                                           StringId32 componentTypeId,
                                           const std::any& component ) const {
    const IComponentMeta* componentMeta = GetComponentMeta( componentTypeId ).value_or( nullptr );
    if( componentMeta != nullptr ) {
        componentMeta->create( registry, entityId, component );
        return true;
    }

    return false;
}

} // namespace onyx::ecs
