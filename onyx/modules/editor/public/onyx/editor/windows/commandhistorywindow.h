#pragma once 

#include <onyx/ui/imguiwindow.h>

namespace Onyx::InputActions
{
    struct InputActionEvent;
}

namespace Onyx::Editor
{
    class ICommandGraph;

    class CommandHistoryWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "CommandHistory";
        static constexpr StringView WindowCategory = "Window";

        StringView GetWindowId() override { return WindowId; }
    
        void SetCommandQueue(ICommandGraph& commandQueue) { m_CommandStack = &commandQueue; }

    private:
        void OnOpen() override;
        void OnClose() override;
        void OnRender(Ui::ImGuiSystem& imguiSystem) override;

    private:
        void OnUndo(const InputActions::InputActionEvent& undoAction);
        void OnRedo(const InputActions::InputActionEvent& redoAction);

    private:
        ICommandGraph* m_CommandStack = nullptr;
    };  
}