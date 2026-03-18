#pragma once

namespace onyx::physics
{
    enum class CollisionLayer : onyxU16
    {
        STATIC,
        DYNAMIC,
        COUNT,
    };
}