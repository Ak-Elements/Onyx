#include <onyx/editor/windows/commandhistorywindow.h>

#include <onyx/inputactions/inputactionsystem.h>
#include <onyx/editor/commands/commandgraph.h>

#include <imgui.h>
#include <imgui_stacklayout.h>

namespace onyx::editor
{
    void CommandHistoryWindow::OnOpen()
    {
        input_actions::InputActionSystem& inputActionsSystem = GetEngineSystem<input_actions::InputActionSystem>();

        inputActionsSystem.OnInput<&CommandHistoryWindow::OnRedo>("Redo", this);
        inputActionsSystem.OnInput<&CommandHistoryWindow::OnUndo>("Undo", this); 
    }

    void CommandHistoryWindow::OnClose()
    {
        input_actions::InputActionSystem& inputActionsSystem = GetEngineSystem<input_actions::InputActionSystem>();
        inputActionsSystem.Disconnect(this);
    }

    void CommandHistoryWindow::OnRender(ui::ImGuiSystem& imguiSystem)
    {
        Begin();

        ImGui::BeginVertical("##content");

        onyxU32 i = 0;
        for (const UniquePtr<ICommand>& command : m_CommandStack->GetCommands())
        {
            ImGui::BeginHorizontal(format::Format("##item{}", i++));
            ImGui::Text("t    ");
            if (ImGui::Selectable(command->GetCommandId().GetString().data()))
            {
                m_CommandStack->Reset(*command);
            }
            ImGui::EndHorizontal();
        }
        
        ImGui::EndVertical();

        End();
    }

    void CommandHistoryWindow::OnUndo(const input_actions::InputActionEvent& undoAction)
    {
        const ui::ImGuiWindow* parent = GetParent<ImGuiWindow>().value_or(nullptr);
        ONYX_ASSERT(parent != nullptr);

        if ((IsFocused() == false) && (parent->IsFocused() == false))
            return;

        if (undoAction.GetData<bool>() == false)
            return;
        
        m_CommandStack->MoveBack();
    }

    void CommandHistoryWindow::OnRedo(const input_actions::InputActionEvent& redoAction)
    {
        const ui::ImGuiWindow* parent = GetParent<ImGuiWindow>().value_or(nullptr);
        ONYX_ASSERT(parent != nullptr);

        if ((IsFocused() == false) && (parent->IsFocused() == false))
            return;

        if (redoAction.GetData<bool>() == false)
            return;

        m_CommandStack->MoveForward();
    }
}