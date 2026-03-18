#pragma once 

#include <onyx/ui/imguiwindow.h>

namespace onyx::input_actions
{
    struct InputActionEvent;
}

namespace onyx::editor
{
    class ICommandGraph;

    class CommandHistoryWindow : public ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "CommandHistory";
        static constexpr StringView WindowCategory = "Window";

        StringView GetWindowId() override { return WindowId; }
    
        void SetCommandQueue(ICommandGraph& commandQueue) { m_CommandStack = &commandQueue; }

    private:
        void OnOpen() override;
        void OnClose() override;
        void OnRender(ui::ImGuiSystem& imguiSystem) override;

    private:
        void OnUndo(const input_actions::InputActionEvent& undoAction);
        void OnRedo(const input_actions::InputActionEvent& redoAction);

    private:
        ICommandGraph* m_CommandStack = nullptr;
    };  
}