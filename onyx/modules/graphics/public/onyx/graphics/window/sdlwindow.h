#pragma once

#if ONYX_USE_SDL2
#include <SDL_events.h>

struct SDL_Window;

namespace Onyx
{
    enum class WindowMode : onyxU8;

    class SdlWindow
    {
        using ResizeCallback = InplaceFunction<void(onyxU32, onyxU32)>;
    public:
        SdlWindow() = default;
        ~SdlWindow();

        void Create(const std::string_view& windowTitle, onyxU32 width, onyxU32 height);
        void Destroy();

        void Update();

        void Show();
        void Hide();

        SDL_Window* GetWindowHandle() const { return m_WindowHandle; }

        void SetTitle(const std::string_view& m_title);

        void GetFrameBufferSize(onyxU32& width, onyxU32& height) const;

        void Resize(onyxU32 width, onyxU32 height);
        void SetWindowMode(WindowMode mode);

        bool GetRequiredExtensions(std::vector<const char*>& outExtensions) const;

        template <typename... Args>
        void SetOnResizeCallback(Args&&... args) { m_OnResizeCallback = ResizeCallback(std::forward<Args>(args)...); }

    private:
        ResizeCallback m_OnResizeCallback;
        SDL_Window* m_WindowHandle = nullptr;
    };
}
#endif