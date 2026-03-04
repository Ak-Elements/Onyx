#pragma once

namespace Onyx::Physics
{
    enum class BroadPhaseLayer : onyxU8
    {
        STATIC,
        DYNAMIC,
        COUNT,
    };
}