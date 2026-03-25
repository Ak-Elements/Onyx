#include <onyx/rhi/vulkan/surface.h>

#include <onyx/rhi/vulkan/instance.h>

#include <onyx/platform/platformcontext.h>

#if ONYX_USE_X11
#include <vulkan/vulkan_xcb.h>
#endif

namespace onyx::rhi::vulkan {
Surface::Surface( const Instance& instance, const platform::Window& window )
    : m_Instance( instance ) {
#if ONYX_USE_SDL2
    if ( !SDL_Vulkan_CreateSurface( window.GetWindowHandle(), instance.GetHandle(), &m_Surface ) ) {
        ONYX_LOG_ERROR( "Failed creating SDL surface" );
    }
#elif ONYX_IS_WINDOWS
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = window.GetContext().GetInstanceHandle();
    surfaceCreateInfo.hwnd = window.GetWindowHandle();
    VK_CHECK_RESULT( vkCreateWin32SurfaceKHR( instance.GetHandle(), &surfaceCreateInfo, nullptr, &m_Surface ) );
#elif ONYX_IS_UNIX && ONYX_USE_WAYLAND
    VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.display = window.GetContext().GetDisplayHandle();
    surfaceCreateInfo.surface = window.GetSurfaceHandle();
    VK_CHECK_RESULT( vkCreateWaylandSurfaceKHR( instance.GetHandle(), &surfaceCreateInfo, nullptr, &m_Surface ) );
#elif ONYX_IS_UNIX && ONYX_USE_X11
    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.connection = window.GetContext().getConnectionHandle();
    surfaceCreateInfo.window = window.GetSurfaceHandle();
    VK_CHECK_RESULT( vkCreateXcbSurfaceKHR( instance.GetHandle(), &surfaceCreateInfo, nullptr, &m_Surface ) );
#else
    static_assert( "Unhandled window for surface creation." );
#endif
}

Surface::~Surface() {
    vkDestroySurfaceKHR( m_Instance.GetHandle(), m_Surface, nullptr );
    m_Surface = nullptr;
}
} // namespace onyx::rhi::vulkan
