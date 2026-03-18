#include <onyx/editor/windows/editorwindow.h>

namespace onyx::editor
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
