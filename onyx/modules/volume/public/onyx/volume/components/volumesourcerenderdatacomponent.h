#pragma once

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/scopeddisable.h>
#endif

namespace Onyx::Volume
{
    struct VolumeSourceRenderDataComponent
    {
        static constexpr StringId32 TypeId = "Onyx::Volume::Components::VolumeSourceRenderDataComponent";
        StringId32 GetTypeId() const { return TypeId; }

        static constexpr bool IsTransient = true;
        static constexpr bool HideInEditor = true;

        onyxU32 Offset; // offset into the runtime ssbo buffer

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        bool DrawImGuiEditor()
        {
            Ui::ScopedImGuiDisabled disable;
            Ui::PropertyGrid::DrawProperty("SSBO Index", Offset);
            return true;
        }
#endif
    };
}
