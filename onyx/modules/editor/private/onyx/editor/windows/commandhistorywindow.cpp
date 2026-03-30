#include <onyx/editor/windows/commandhistorywindow.h>

#include <onyx/editor/commands/commandgraph.h>
#include <onyx/inputactions/inputactionsystem.h>

#include <imgui.h>
#include <imgui_stacklayout.h>

namespace onyx::editor {
void CommandHistoryWindow::onOpen() {
    input_actions::InputActionSystem& inputActionsSystem = getEngineSystem< input_actions::InputActionSystem >();

    inputActionsSystem.OnInput< &CommandHistoryWindow::OnRedo >( "Redo", this );
    inputActionsSystem.OnInput< &CommandHistoryWindow::OnUndo >( "Undo", this );
}

void CommandHistoryWindow::onClose() {
    input_actions::InputActionSystem& inputActionsSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionsSystem.Disconnect( this );
}

void CommandHistoryWindow::onRender( ui::ImGuiSystem& imguiSystem ) {
    // begin();

    ImGui::BeginVertical( "##content" );

    uint32_t i = 0;
    for ( const UniquePtr< ICommand >& command : m_CommandStack->GetCommands() ) {
        ImGui::BeginHorizontal( format::format( "##item{}", i++ ) );
        ImGui::Text( "t    " );
        if ( ImGui::Selectable( command->GetCommandId().getString().data() ) ) {
            m_CommandStack->Reset( *command );
        }
        ImGui::EndHorizontal();
    }

    ImGui::EndVertical();

    // end();
}

void CommandHistoryWindow::OnUndo( const input_actions::InputActionEvent& undoAction ) {
    const ui::ImGuiWindow* parent = getParent< ImGuiWindow >().value_or( nullptr );
    ONYX_ASSERT( parent != nullptr );

    if ( ( isFocused() == false ) && ( parent->isFocused() == false ) )
        return;

    if ( undoAction.GetData< bool >() == false )
        return;

    m_CommandStack->MoveBack();
}

void CommandHistoryWindow::OnRedo( const input_actions::InputActionEvent& redoAction ) {
    const ui::ImGuiWindow* parent = getParent< ImGuiWindow >().value_or( nullptr );
    ONYX_ASSERT( parent != nullptr );

    if ( ( isFocused() == false ) && ( parent->isFocused() == false ) )
        return;

    if ( redoAction.GetData< bool >() == false )
        return;

    m_CommandStack->MoveForward();
}
} // namespace onyx::editor
