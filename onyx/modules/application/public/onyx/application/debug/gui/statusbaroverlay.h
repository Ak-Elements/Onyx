#pragma once

#if ONYX_UI_MODULE
#include <onyx/ui/imguiwindow.h>

namespace Onyx::Application::Debug
{
    class StatusBarOverlayItem
    {
    public:
        virtual ~StatusBarOverlayItem( ) = default;
        virtual StringView GetValue() = 0;
        virtual StringView GetName() = 0;
    };

    class StatusBarOverlay : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "StatusBar";

        StringView GetWindowId() override { return WindowId; }

        template <typename T, class... _Types>
        T& AddOverlay(_Types&&... args)
        {
            return static_cast<T&>(*Items.emplace_back(MakeUnique<T>(std::forward<_Types>(args)...)));
        }
    private:
        void OnRender(Ui::ImGuiSystem& system) override;
    private:
        DynamicArray<UniquePtr<StatusBarOverlayItem>> Items;
        Vector2u32 Position;
    };
}

#endif