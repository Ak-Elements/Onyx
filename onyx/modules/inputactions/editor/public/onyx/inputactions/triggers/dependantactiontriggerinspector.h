#pragma once

#include <onyx/inputactions/triggers/dependantactiontrigger.h>
#include <onyx/ui/propertyinspector.h>
namespace onyx::ui {
template <>
struct PropertyInspector< input_actions::DependantActionInputTrigger > {
    static bool draw( input_actions::DependantActionInputTrigger& trigger, bool /*forceShow*/ );
};
} // namespace onyx::ui
