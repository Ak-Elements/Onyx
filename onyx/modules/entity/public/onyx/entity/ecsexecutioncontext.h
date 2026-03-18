#pragma once

#include <onyx/engine/enginesystem.h>
#include <onyx/entity/entityregistry.h>

namespace onyx::ecs
{
    struct ECSExecutionContext
    {
        DeltaGameTime DeltaTime;
        EntityRegistry& Registry;
        IEngine& Engine;
    };
}