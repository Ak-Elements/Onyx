#pragma once

#include <onyx/ui/propertyinspector.h>
#include <onyx/inputactions/triggers/dependantactiontrigger.h>
namespace Onyx::Ui
{
    template <>
    struct PropertyInspector<InputActions::DependantActionInputTrigger>
    {
        static bool Draw(InputActions::DependantActionInputTrigger& trigger, bool /*forceShow*/);
    };
}

