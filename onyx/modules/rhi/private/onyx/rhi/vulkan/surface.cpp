#include <onyx/rhi/vulkan/surface.h>

#include <onyx/rhi/vulkan/instance.h>

#include <onyx/platform/platformcontext.h>

#if ONYX_USE_X11
#include <vulkan/vulkan_xcb.h>
#endif

namespace onyx::rhi::vulkan {
Surface::Surface( const Instance& instance, const platform::Window& window )
    : m_instance( instance ) {
#if ONYX_IS_WINDOWS
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = window.getContext().getInstanceHandle();
    surfaceCreateInfo.hwnd = window.getWindowHandle();
    VK_CHECK_RESULT( vkCreateWin32SurfaceKHR( instance.getHandle(), &surfaceCreateInfo, nullptr, &m_surface ) );
#elif ONYX_IS_UNIX && ONYX_USE_WAYLAND
    VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.display = window.getContext().getDisplayHandle();
    surfaceCreateInfo.surface = window.getSurfaceHandle();
    VK_CHECK_RESULT( vkCreateWaylandSurfaceKHR( instance.getHandle(), &surfaceCreateInfo, nullptr, &m_surface ) );
#elif ONYX_IS_UNIX && ONYX_USE_X11
    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.connection = window.getContext().getConnectionHandle();
    surfaceCreateInfo.window = window.getSurfaceHandle();
    VK_CHECK_RESULT( vkCreateXcbSurfaceKHR( instance.getHandle(), &surfaceCreateInfo, nullptr, &m_surface ) );
#else
    static_assert( "Unhandled window for surface creation." );
#endif
}

Surface::~Surface() {
    vkDestroySurfaceKHR( m_instance.getHandle(), m_surface, nullptr );
    m_surface = nullptr;
}
} // namespace onyx::rhi::vulkan
