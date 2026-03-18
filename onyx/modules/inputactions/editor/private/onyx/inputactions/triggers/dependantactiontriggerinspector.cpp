#include <onyx/inputactions/triggers/dependantactiontriggerinspector.h>

#include <onyx/ui/propertygrid.h>

namespace onyx::ui
{
    /*static*/ bool PropertyInspector<input_actions::DependantActionInputTrigger>::Draw(input_actions::DependantActionInputTrigger& trigger, bool /*forceShow*/)
    {
        bool isModified = false;
        String actionId (trigger.GetActionId().GetString() );
        if( property_grid::DrawProperty("Action", actionId) )
        {
            trigger.SetActionId(StringId64(actionId));
            isModified = true;
        }
        
        return isModified;
    }
}
