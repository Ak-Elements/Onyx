#include <onyx/graphics/vulkan/surface.h>
#include <onyx/graphics/vulkan/instance.h>
#include <onyx/graphics/window.h>

#if ONYX_USE_SDL2
#include <SDL_vulkan.h>
#endif

namespace Onyx::Graphics::Vulkan
{
    Surface::Surface(const Instance& instance, const Window& window)
        : m_Instance(instance)
    {
#if ONYX_USE_SDL2
        if (!SDL_Vulkan_CreateSurface(window.GetWindowHandle(), instance.GetHandle(), &m_Surface))
        {
            ONYX_LOG_ERROR("Failed creating SDL surface");
        }
#elif ONYX_IS_WINDOWS
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.hinstance = ::GetModuleHandle(nullptr);
        surfaceCreateInfo.hwnd = window.GetWindowHandle();
        VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(instance.GetHandle(), &surfaceCreateInfo, nullptr, &m_Surface));
#else
        static_assert("Unhandled window for surface creation.");
#endif
    }

    Surface::~Surface()
    {
        vkDestroySurfaceKHR(m_Instance.GetHandle(), m_Surface, nullptr);
        m_Surface = nullptr;
    }
}
