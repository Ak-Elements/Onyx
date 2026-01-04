#include <onyx/graphics/vulkan/surface.h>
#include <onyx/graphics/vulkan/instance.h>

#if ONYX_IS_UNIX && ONYX_USE_WAYLAND
#include <onyx/platform/platformcontext.h>
#endif

namespace Onyx::Graphics::Vulkan
{
    Surface::Surface(const Instance& instance, const Platform::Window& window)
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
#elif ONYX_IS_UNIX && ONYX_USE_WAYLAND
        VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
    	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	    surfaceCreateInfo.display = window.GetContext()->GetDisplayHandle();
	    surfaceCreateInfo.surface = window.GetSurfaceHandle();
	    VK_CHECK_RESULT(vkCreateWaylandSurfaceKHR(instance.GetHandle(), &surfaceCreateInfo, nullptr, &m_Surface));
#elif defined(VK_USE_PLATFORM_XCB_KHR)


        static_assert("Unhandled window for surface creation.");
#endif
    }

    Surface::~Surface()
    {
        vkDestroySurfaceKHR(m_Instance.GetHandle(), m_Surface, nullptr);
        m_Surface = nullptr;
    }
}
