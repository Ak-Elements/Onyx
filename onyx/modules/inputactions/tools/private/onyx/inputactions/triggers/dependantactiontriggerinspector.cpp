#include <onyx/inputactions/triggers/dependantactiontriggerinspector.h>

#include <onyx/ui/propertygrid.h>

namespace onyx::ui {
/*static*/ bool PropertyInspector< input_actions::DependantActionInputTrigger >::draw(
    input_actions::DependantActionInputTrigger& trigger,
    bool /*forceShow*/ ) {
    bool isModified = false;
    String actionId( trigger.GetActionId().getString() );
    if ( property_grid::drawProperty( "Action", actionId ) ) {
        trigger.SetActionId( StringId64( actionId ) );
        isModified = true;
    }

    return isModified;
}
} // namespace onyx::ui
