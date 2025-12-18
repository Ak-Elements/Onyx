#include <onyx/gamecore/components/graphics/lightcomponents.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool DirectionalLightComponent::DrawProperties(bool /*showHidden*/)
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawProperty("Shadow Amount", Light.ShadowAmount);
        hasModified |= Ui::PropertyGrid::DrawProperty("Cast Shadows", Light.IsShadowCasting);
        return hasModified;
    }

    bool PointLightComponent::DrawProperties(bool /*showHidden*/)
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawProperty("Radius", Light.Radius);

        bool isEnabled = Light.IsEnabled == 1;
        hasModified |= Ui::PropertyGrid::DrawProperty("Is Enabled", isEnabled);
        Light.IsEnabled = isEnabled == 1;

        bool isShadowCasting = Light.IsShadowCasting == 1;
        hasModified |= Ui::PropertyGrid::DrawProperty("Cast Shadows", isShadowCasting);
        Light.IsShadowCasting = isShadowCasting == 1;
        return hasModified;
    }

    bool SpotLightComponent::DrawProperties(bool /*showHidden*/)
    {
        bool hasModified = Ui::PropertyGrid::DrawColorProperty("Color", Light.Color);

        hasModified |= Ui::PropertyGrid::DrawProperty("Intensity", Light.Intensity);
        hasModified |= Ui::PropertyGrid::DrawProperty("Falloff", Light.Falloff);
        hasModified |= Ui::PropertyGrid::DrawProperty("Range", Light.Range);
        hasModified |= Ui::PropertyGrid::DrawProperty("Angle", Light.Angle);
        hasModified |= Ui::PropertyGrid::DrawProperty("Angle Attenuation", Light.AngleAttenuation);
        hasModified |= Ui::PropertyGrid::DrawProperty("Cast Shadows", Light.IsShadowCasting);
        return hasModified;
    }
#endif
}
