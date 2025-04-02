#include <onyx/application/debug/gui/statusbaroverlay.h>

#if ONYX_UI_MODULE

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>

namespace Onyx::Application::Debug
{
    void StatusBarOverlay::OnRender(Ui::ImGuiSystem& /*system*/)
    {
        ::ImGuiWindow* window = ImGui::FindWindowByName("SceneView");
        if (window != nullptr)
        {
            ImGui::SetNextWindowPos(window->InnerRect.Min + ImGui::GetStyle().FramePadding);
            ImGui::SetNextWindowSize(window->Size);
        }
        else
        {
            return;
        }
        
        ImGui::Begin("##debugOverlay", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav);

        ImGui::BeginHorizontal("items");

        bool hasOverlayClosed = false;
        for (UniquePtr<StatusBarOverlayItem>& overlay : Items)
        {
            const StringView overlayName = overlay->GetName();
            const StringView value = overlay->GetValue();

            bool hasClicked = ImGui::Button(value.data(), ImVec2(100,0));

            if (hasClicked)
            {
                bool open = true;
                ImGui::Begin(overlayName.data(), &open);
                ImGui::End();
            }
        }

        ImGui::EndHorizontal();

        ImGui::End();

        if (hasOverlayClosed)
        {
            std::erase_if(Items, [](const UniquePtr<StatusBarOverlayItem>& overlay) { return overlay == nullptr; });
        }
    }
}

#endif