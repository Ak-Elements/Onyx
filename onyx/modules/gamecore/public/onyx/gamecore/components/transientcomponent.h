#pragma once

namespace Onyx::GameCore
{
    // Marks the whole entity as transient, which prevents saving the entity to scene files
    struct TransientComponent
    {
        static constexpr bool HideInEditor = true;
        static constexpr bool IsTransient = true;
    };
}