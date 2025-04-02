#include <editor/windows/editorwindow.h>

namespace Onyx::Editor
{
    void EditorWindow::Render()
    {
        if (m_IsOpen == false)
            return;

        OnRender();
    }

    void EditorWindow::Open()
    {
        m_IsOpen = true;
    }

    void EditorWindow::Close()
    {
        m_IsOpen = false;
    }
}
