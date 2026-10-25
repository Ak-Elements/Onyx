#include <onyx/editor/windows/commandhistorywindow.h>

#include <onyx/editor/commands/commandgraph.h>
#include <onyx/inputactions/inputactionsystem.h>

#include <imgui.h>
#include <imgui_stacklayout.h>

namespace onyx::editor {
void CommandHistoryWindow::onOpen() {
    input_actions::InputActionSystem& inputActionsSystem = getEngineSystem< input_actions::InputActionSystem >();

    inputActionsSystem.onInput< &CommandHistoryWindow::onRedo >( "Redo", this );
    inputActionsSystem.onInput< &CommandHistoryWindow::onUndo >( "Undo", this );
}

void CommandHistoryWindow::onClose() {
    input_actions::InputActionSystem& inputActionsSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionsSystem.disconnect( this );
}

void CommandHistoryWindow::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    // begin();

    ImGui::BeginVertical( "##content" );

    uint32_t i = 0;
    for( const UniquePtr< ICommand >& command : m_commandStack->getCommands() ) {
        ImGui::BeginHorizontal( format::format( "##item{}", i++ ).data() );
        ImGui::Text( "t    " );
        if( ImGui::Selectable( command->getCommandId().getString().data() ) ) {
            m_commandStack->reset( *command );
        }
        ImGui::EndHorizontal();
    }

    ImGui::EndVertical();

    // end();
}

void CommandHistoryWindow::onUndo( const input_actions::InputActionEvent& undoAction ) {
    const ui::ImGuiWindow* parent = getParent< ImGuiWindow >().value_or( nullptr );
    ONYX_ASSERT( parent != nullptr );

    if( ( isFocused() == false ) && ( parent->isFocused() == false ) )
        return;

    if( undoAction.getData< bool >() == false )
        return;

    m_commandStack->moveBack();
}

void CommandHistoryWindow::onRedo( const input_actions::InputActionEvent& redoAction ) {
    const ui::ImGuiWindow* parent = getParent< ImGuiWindow >().value_or( nullptr );
    ONYX_ASSERT( parent != nullptr );

    if( ( isFocused() == false ) && ( parent->isFocused() == false ) )
        return;

    if( redoAction.getData< bool >() == false )
        return;

    m_commandStack->moveForward();
}
} // namespace onyx::editor
