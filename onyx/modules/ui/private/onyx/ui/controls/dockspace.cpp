#include <onyx/ui/controls/dockspace.h>

#include <imgui_internal.h>

namespace Onyx::Ui
{
    Dockspace Dockspace::Create(const DynamicArray<DockSplit>& splits)
    {
        Dockspace dockspace;
        dockspace.shouldReset = true;
        dockspace.splits = splits;
        return dockspace;
    }

    bool Dockspace::Render(ImGuiID dockspaceId)
    {
        if (shouldReset)
        {
            shouldReset = false;
            
            // Setup dockspace
            ImGui::DockBuilderRemoveNode(dockspaceId);

            ImGuiID currentNode = ImGui::DockBuilderAddNode(dockspaceId);
            ImGui::DockBuilderSetNodePos(currentNode, ImVec2(0, 0));
            ImGui::DockBuilderSetNodeSize(currentNode, ImGui::GetWindowSize());

            for (const DockSplit& split : splits)
            {
                ImGuiDir direction = ImGuiDir_None;
                switch (split.Direction)
                {
                    case Left: direction = ImGuiDir_Left; break;
                    case Right: direction = ImGuiDir_Right; break;
                    case Up: direction = ImGuiDir_Up; break;
                    case Down: direction = ImGuiDir_Down; break;
                }

                ImGuiID nodeInDirection, nodeOppositeDirection;
                ImGui::DockBuilderSplitNode(currentNode, direction, split.Ratio, &nodeInDirection, &nodeOppositeDirection);

                if (split.DockWindowInDirection.empty() == false)
                {
                    ImGui::DockBuilderDockWindow(split.DockWindowInDirection.c_str(), nodeInDirection);
                }

                if (split.DockWindowOppositeDirection.empty() == false)
                {
                    ImGui::DockBuilderDockWindow(split.DockWindowOppositeDirection.c_str(), nodeOppositeDirection);
                }

                currentNode = nodeInDirection;
            }

            ImGui::DockBuilderFinish(dockspaceId);
        }

        ImGui::DockSpace(dockspaceId, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode, windowClass);
        return true;
    }

}
