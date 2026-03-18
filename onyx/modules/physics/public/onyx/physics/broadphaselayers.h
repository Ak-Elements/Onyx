#pragma once

namespace onyx::physics
{
    enum class BroadPhaseLayer : onyxU8
    {
        STATIC,
        DYNAMIC,
        COUNT,
    };
}