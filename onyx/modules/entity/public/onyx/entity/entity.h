#pragma once

#include <entt/entity/entity.hpp>

#include <onyx/onyx_types.h>

namespace Onyx::Entity
{
    class EntityRegistry;
    enum class EntityId : onyxU32;

    struct Entity
    {
        Entity() = default;
        ~Entity() = default;

        Entity(EntityId entity, EntityRegistry* registry)
            : m_EntityId(entity)
            , m_Registry(registry)
        {
        }

        EntityId GetId() { return m_EntityId; }
        EntityId GetId() const { return m_EntityId; }

    private:
        EntityId m_EntityId = entt::null;
        EntityRegistry* m_Registry = nullptr;
    };
}
