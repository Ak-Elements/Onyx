#include <onyx/gamecore/components/freecameracomponent.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
    void FreeCameraControllerComponent::DrawImGuiEditor()
    {
        Ui::PropertyGrid::DrawScalarProperty("Base Velocity", BaseVelocity);
        Ui::PropertyGrid::DrawScalarProperty("Min Velocity", MinVelocity);
        Ui::PropertyGrid::DrawScalarProperty("Max Velocity", MaxVelocity);

        Ui::PropertyGrid::DrawScalarProperty("Rotation Velocity", RotationVelocity);
        Ui::PropertyGrid::DrawScalarProperty("Velocity Increment", VelocityIncrementFactor);
    }
}
