#include <onyx/gamecore/components/freecameracomponent.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
    bool FreeCameraControllerComponent::DrawProperties(bool /*showHidden*/)
    {
        bool isModified = Ui::PropertyGrid::DrawProperty("Base Velocity", BaseVelocity);
        isModified |= Ui::PropertyGrid::DrawProperty("Min Velocity", MinVelocity);
        isModified |= Ui::PropertyGrid::DrawProperty("Max Velocity", MaxVelocity);

        isModified |= Ui::PropertyGrid::DrawProperty("Rotation Velocity", RotationVelocity);
        isModified |= Ui::PropertyGrid::DrawProperty("Velocity Increment", VelocityIncrementFactor);

        return isModified;
    }
}
