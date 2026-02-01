#pragma once

#include <onyx/engine/enginesystem.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace Onyx::Ui
{
    class ImGuiSystem;

    enum class WindowPosition
    {
        TopLeft,
        TopCenter,
        TopRight,
        CenterLeft,
        Center,
        CenterRight,
        BottomLeft,
        BottomCenter,
        BottomRight
    };

    class ImGuiWindow
    {
    public:
        virtual ~ImGuiWindow() = default;

        void Open();
        void Close();

        void Render(ImGuiSystem& imguiSystem);

        virtual StringView GetWindowId() = 0;
        virtual constexpr StringId32 GetWindowCategory() { return "Default"; }

        void SetName(const String& newName) { m_Name = newName; }

        bool IsOpen() const { return m_IsOpen; }
        bool IsCollapsed() const { return m_IsCollapsed; }
        void SetIsCollapsed(bool _isCollapsed);
        bool IsDocked() const { return m_IsDocked; }

        void BringToFront();

        void SetEngine(IEngine& engine) { m_Engine = &engine; }

    protected:
        bool Begin();
        void End();

        bool BeginMenuBar();
        void EndMenuBar();

        void SetWindowFlags(ImGuiWindowFlags newFlags) { m_Flags = newFlags; }
        ImGuiWindowFlags GetWindowFlags() const { return m_Flags; }

        void SetPosition(Vector2s32 position);
        void SetPosition(Vector2s32 position, Vector2f32 pivot);

        void SetDefaultPosition(Vector2s32 position);
        void SetDefaultPosition(Vector2s32 position, Vector2f32 pivot);
        void SetDefaultPosition(WindowPosition position);

        const ImGuiWindowClass& GetWindowClass() const { return m_WindowClass; }

        template <typename T>
        T& GetEngineSystem() { return m_Engine->GetSystem<T>(); }

        template <typename T>
        const T& GetEngineSystem() const { return m_Engine->GetSystem<T>(); }

    private:
        virtual void OnOpen();
        virtual void OnRender(ImGuiSystem& imguiSystem) = 0;
        virtual void OnClose();

        virtual void OnRenderMainMenuBar();

    private:
        String m_Name;
        ImGuiWindowClass m_WindowClass;
        ImGuiWindowFlags m_Flags;

        IEngine* m_Engine = nullptr;

        bool m_IsDocked = false;
        bool m_IsOpen = false;
        bool m_IsCollapsed = false;
    };
}