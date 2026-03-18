#pragma once

#include <onyx/ui/propertyinspector.h>
#include <onyx/inputactions/triggers/dependantactiontrigger.h>
namespace onyx::ui
{
    template <>
    struct PropertyInspector<input_actions::DependantActionInputTrigger>
    {
        static bool Draw(input_actions::DependantActionInputTrigger& trigger, bool /*forceShow*/);
    };
}

