#include <onyx/ui/controls/button.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <onyx/geometry/rect2.h>
#include <onyx/localization/localizedstring.h>

namespace Onyx::Ui
{
    bool Button(const Localization::LocalizedString& localizedLabel)
    {
        return Button(localizedLabel.Get());
    }

    bool Button(const Localization::LocalizedString& localizedLabel, Vector2f size)
    {
        return Button(localizedLabel.Get(), size);
    }

    bool Button(StringView label)
    {
        return ImGui::Button(label.data());
    }

    bool Button(StringView label, Vector2f size)
    {
        return ImGui::Button(label.data(), ImVec2(size[0], size[1]));
    }

    bool Selectable(const Localization::LocalizedString& localizedLabel, bool isSelected)
    {
        return Selectable(localizedLabel.Get(), isSelected, false, ImGuiSelectableFlags_None);
    }

    bool Selectable(const Localization::LocalizedString& localizedLabel, bool isSelected, bool shouldFocusOnSelected)
    {
        return Selectable(localizedLabel.Get(), isSelected, shouldFocusOnSelected, ImGuiSelectableFlags_None);
    }

    bool Selectable(StringView label, bool isSelected)
    {
        return Selectable(label, isSelected, false, ImGuiSelectableFlags_None);
    }

    bool Selectable(StringView label, bool isSelected, bool shouldFocusOnSelected)
    {
        return Selectable(label, isSelected, shouldFocusOnSelected, ImGuiSelectableFlags_None);
    }

    bool Selectable(StringView label, bool isSelected, bool shouldFocusOnSelected, onyxS32 flags)
    {
        isSelected = ImGui::Selectable(label.data(), isSelected, flags);

        if (isSelected && shouldFocusOnSelected)
        {
            ImGui::SetItemDefaultFocus();
        }

        return isSelected;
    }

    ButtonState ButtonBehavior(onyxS32 id, const ImRect& boundingBox)
    {
        ImVec2 min = boundingBox.GetBL();
        ImVec2 size = boundingBox.GetSize();
        return ButtonBehavior(id, Rect2f32{ min.x, min.y, size.x, size.y }, ImGuiButtonFlags_None);
    }

    ButtonState ButtonBehavior(onyxS32 id, const ImRect& boundingBox, onyxS32 flags)
    {
        ImVec2 min = boundingBox.GetBL();
        ImVec2 size = boundingBox.GetSize();
        return ButtonBehavior(id, Rect2f32{ min.x, min.y, size.x, size.y }, flags);
    }

    ButtonState ButtonBehavior(onyxS32 id, const Rect2f32& boundingBox)
    {
        return ButtonBehavior(id, boundingBox, ImGuiButtonFlags_None);
    }

    ButtonState ButtonBehavior(onyxS32 id, const Rect2f32& boundingBox, onyxS32 flags)
    {
        bool isHeld = false;
        bool isHovered = false;

        const Vector2f& min = boundingBox.GetBottomLeft();
        const Vector2f& max = boundingBox.GetTopRight();

        ImRect imguiBounds{ min[0], min[1], max[0], max[1] };
        bool isPressed = ImGui::ButtonBehavior(imguiBounds, id, &isHovered, &isHeld, flags);

        return isPressed ? ButtonState::Pressed :
            isHeld ? ButtonState::Held :
            isHovered ? ButtonState::Hovered : ButtonState::None;
    }
}

