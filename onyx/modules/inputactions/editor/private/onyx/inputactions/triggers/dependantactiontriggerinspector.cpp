#include <onyx/inputactions/triggers/dependantactiontriggerinspector.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::Ui
{
    /*static*/ bool PropertyInspector<InputActions::DependantActionInputTrigger>::Draw(InputActions::DependantActionInputTrigger& trigger, bool /*forceShow*/)
    {
        bool isModified = false;
        String actionId (trigger.GetActionId().GetString() );
        if( PropertyGrid::DrawProperty("Action", actionId) )
        {
            trigger.SetActionId(StringId64(actionId));
            isModified = true;
        }
        
        return isModified;
    }
}
