#include <onyx/gamecore/components/graphics/lightcomponents.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void DirectionalLightComponent::DrawImGuiEditor()
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Shadow Amount", Light.ShadowAmount);
        hasModified |= Ui::PropertyGrid::DrawBoolProperty("Cast Shadows", Light.IsShadowCasting);
        ONYX_UNUSED(hasModified);
    }

    void PointLightComponent::DrawImGuiEditor()
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Radius", Light.Radius);

        bool isEnabled = Light.IsEnabled == 1;
        hasModified |= Ui::PropertyGrid::DrawBoolProperty("Is Enabled", isEnabled);
        Light.IsEnabled = isEnabled == 1;

        bool isShadowCasting = Light.IsShadowCasting == 1;
        hasModified |= Ui::PropertyGrid::DrawBoolProperty("Cast Shadows", isShadowCasting);
        Light.IsShadowCasting = isShadowCasting == 1;
        ONYX_UNUSED(hasModified);
    }

    void SpotLightComponent::DrawImGuiEditor()
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Falloff", Light.Falloff);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Range", Light.Range);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Angle", Light.Angle);
        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Angle Attenuation", Light.AngleAttenuation);
        hasModified |= Ui::PropertyGrid::DrawBoolProperty("Cast Shadows", Light.IsShadowCasting);
        ONYX_UNUSED(hasModified);
    }
#endif
}
