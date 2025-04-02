#pragma once

#include <onyx/onyx_types.h>

#include <entt/core/type_info.hpp>

namespace Onyx::Entity
{
    template <typename T>
    inline constexpr onyxU32 GetHash()
    {
        return entt::type_hash<T>::value();
    }
}
