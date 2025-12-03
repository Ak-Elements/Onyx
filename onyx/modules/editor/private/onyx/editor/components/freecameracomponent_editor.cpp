#include <onyx/gamecore/components/freecameracomponent.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
    bool FreeCameraControllerComponent::DrawImGuiEditor()
    {
        bool isModified = Ui::PropertyGrid::DrawScalarProperty("Base Velocity", BaseVelocity);
        isModified |= Ui::PropertyGrid::DrawScalarProperty("Min Velocity", MinVelocity);
        isModified |= Ui::PropertyGrid::DrawScalarProperty("Max Velocity", MaxVelocity);

        isModified |= Ui::PropertyGrid::DrawScalarProperty("Rotation Velocity", RotationVelocity);
        isModified |= Ui::PropertyGrid::DrawScalarProperty("Velocity Increment", VelocityIncrementFactor);

        return isModified;
    }
}
