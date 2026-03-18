#pragma once

#include <onyx/engine/enginesystem.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace onyx::ui
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

        virtual constexpr StringId32 GetWindowCategory() { return "Default"; }

        void SetWindowId(String windowId) { m_Id = windowId; }
        virtual StringView GetWindowId() { return m_Id; }

        void SetName(const String& newName) { m_Name = newName; }
        void SetWindowClass(ImGuiWindowClass* windowClass) { m_WindowClass = windowClass; }

        void SetEngine(IEngine& engine) { m_Engine = &engine; }
        void SetParent(ImGuiWindow& parent)  { m_Parent = &parent; }

        bool IsOpen() const { return m_IsOpen; }
        bool IsCollapsed() const { return m_IsCollapsed; }
        void SetIsCollapsed(bool _isCollapsed);
        bool IsDocked() const { return m_IsDocked; }
        bool IsFocused() const { return m_IsFocused; }

        void BringToFront();

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

        const ImGuiWindowClass& GetWindowClass() const { return *m_WindowClass; }

        template <typename T>
        T& GetEngineSystem() { return m_Engine->GetSystem<T>(); }

        template <typename T>
        const T& GetEngineSystem() const { return m_Engine->GetSystem<T>(); }

        template <typename T>
        const Optional<T*> GetParent()
        {
            if ( ImGuiWindow* parent = m_Parent.value_or(nullptr))
            {
                return static_cast<T*>(parent); 
            }
            
            return std::nullopt;
        }

    private:
        virtual void OnOpen();
        virtual void OnRender(ImGuiSystem& imguiSystem) = 0;
        virtual void OnClose();

        virtual void OnRenderMainMenuBar();

    private:
        String m_Id;
        String m_Name;
    
        ImGuiWindowClass* m_WindowClass = nullptr;
        ImGuiWindowFlags m_Flags;
        
        Optional<ImGuiWindow*> m_Parent;
        IEngine* m_Engine = nullptr;

        bool m_IsDocked = false;
        bool m_IsOpen = false;
        bool m_IsCollapsed = false;
        bool m_IsFocused = false;
    };
}