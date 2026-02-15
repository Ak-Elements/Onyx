#include <onyx/entity/componentfactory.h>

namespace Onyx::Entity
{
    bool ComponentFactory::TryCreateComponent(EntityRegistry& registry, EntityId entity, StringId32 componentId) const
    {
        Optional<const IComponentMeta*> componentMetaOptional = GetComponentMeta(componentId);
        const IComponentMeta* componentMeta = *componentMetaOptional;
        componentMeta->Create(registry, entity);
        return true;
    }

    bool ComponentFactory::TryCreateComponent(EntityRegistry& registry, EntityId entity, StringId32 componentTypeId, const Deserializer& deserializer) const
    {
        const IComponentMeta* componentMeta = GetComponentMeta(componentTypeId).value_or(nullptr);
        if (componentMeta != nullptr)
        {
            componentMeta->Create(registry, entity, deserializer);
            return true;
        }

        return false;
    }

    bool ComponentFactory::TryCopyComponent(EntityRegistry& registry, EntityId entityId, StringId32 componentTypeId, void* fromComponentPtr) const
    {
        const IComponentMeta* componentMeta = GetComponentMeta(componentTypeId).value_or(nullptr);
        if (componentMeta != nullptr)
        {
            componentMeta->Copy(registry, entityId, fromComponentPtr);
            return true;
        }

        return false;
    }

    bool ComponentFactory::TryCreateComponent(EntityRegistry& registry, EntityId entityId, StringId32 componentTypeId, Span<onyxU32> componentData) const
    {
        const IComponentMeta* componentMeta = GetComponentMeta(componentTypeId).value_or(nullptr);
        if (componentMeta != nullptr)
        {
            componentMeta->Create(registry, entityId, componentData);
            return true;
        }

        return false;
    }
}


