#if ONYX_USE_SDL2
#include <imgui_impl_sdl.h>
#include <onyx/graphics/window/sdlwindow.h>
#include <onyx/graphics/window.h>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace Onyx
{
    SdlWindow::~SdlWindow()
    {
        Destroy();
    }

    void SdlWindow::Create(const std::string_view& windowTitle, onyxU32 width, onyxU32 height)
    {
        //TODO add more params to create e.g.: window mode
        const onyxU32 windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN;
        m_WindowHandle = SDL_CreateWindow(windowTitle.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);

        SDL_AddEventWatch([](void* userData, SDL_Event* event)
        {
            SdlWindow* windowPtr = static_cast<SdlWindow*>(userData);
            if (event->type == SDL_WINDOWEVENT &&
                event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                ONYX_ASSERT(windowPtr !=  nullptr);
                if (windowPtr->m_OnResizeCallback)
                    windowPtr->m_OnResizeCallback(static_cast<onyxU32>(event->window.data1), static_cast<onyxU32>(event->window.data2));
            }
                
                return 0;
        }, this);
    }

    void SdlWindow::Destroy()
    {
        if (m_WindowHandle != nullptr)
        {
            SDL_DestroyWindow(m_WindowHandle);
            m_WindowHandle = nullptr;
        }
    }

    void SdlWindow::Update()
    {
        // needed so sdl context runs as expected
    }

    void SdlWindow::Show()
    {
        SDL_ShowWindow(m_WindowHandle);
    }

    void SdlWindow::Hide()
    {
        SDL_HideWindow(m_WindowHandle);
    }

    void SdlWindow::SetTitle(const std::string_view& title)
    {
        SDL_SetWindowTitle(m_WindowHandle, title.data());
    }

    void SdlWindow::GetFrameBufferSize(onyxU32& outWidth, onyxU32& outHeight) const
    {
#if ONYX_USE_VULKAN
        int width, height;
        SDL_Vulkan_GetDrawableSize(m_WindowHandle, &width, &height);
        outWidth = static_cast<onyxU32>(width);
        outHeight = static_cast<onyxU32>(height);
#else
        static_assert(false, "Unsupported graphics api");s
#endif
    }

    void SdlWindow::Resize(onyxU32 width, onyxU32 height)
    {
        SDL_SetWindowSize(m_WindowHandle, width, height);
    }

    void SdlWindow::SetWindowMode(WindowMode mode)
    {
        onyxU32 modeFlags = 0;
        switch (mode)
        {
        case WindowMode::Windowed:
            modeFlags = 0;
            break;
        case WindowMode::Borderless:
            modeFlags = SDL_WINDOW_BORDERLESS;
            break;
        case WindowMode::Fullscreen:
            modeFlags = SDL_WINDOW_FULLSCREEN;
            break;
        }

        SDL_SetWindowFullscreen(m_WindowHandle, modeFlags);
    }

    bool SdlWindow::GetRequiredExtensions(std::vector<const char*>& outExtensions) const
    {
#if ONYX_USE_VULKAN
        onyxU32 extensionCount = 0;
        if (!SDL_Vulkan_GetInstanceExtensions(m_WindowHandle, &extensionCount, nullptr))
        {
            //std::cout << "Unable to query the number of Vulkan instance extensions\n";
            return false;
        }

        outExtensions.resize(extensionCount);
        if (!SDL_Vulkan_GetInstanceExtensions(m_WindowHandle, &extensionCount, outExtensions.data()))
        {
            //std::cout << "Unable to query the number of Vulkan instance extension names\n";
            return false;
        }
#else
        static_assert(false, "Unsupported graphics api"); s
#endif
        //std::cout << "\n";
        return true;
    }
}
#endif