#pragma once

#include <onyx/engine/enginesystem.h>
#include <onyx/entity/entityregistry.h>

namespace Onyx::Entity
{
    struct ECSExecutionContext
    {
        DeltaGameTime DeltaTime;
        EntityRegistry& Registry;
        IEngine& Engine;
    };
}