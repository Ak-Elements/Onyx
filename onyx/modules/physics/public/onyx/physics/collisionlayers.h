#pragma once

namespace Onyx::Physics
{
    enum class CollisionLayer : onyxU16
    {
        STATIC,
        DYNAMIC,
        COUNT,
    };
}