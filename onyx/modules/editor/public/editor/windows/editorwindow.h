#pragma once

namespace Onyx::Editor
{
    class EditorWindow
    {
    public:
        virtual ~EditorWindow() = default;

        void Render();

        void Open();
        void Close();

        bool IsOpen() const { return m_IsOpen; }

    private:
        virtual void OnRender() {}

    protected:
        bool m_IsOpen = false;
    };
}