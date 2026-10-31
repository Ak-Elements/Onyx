#pragma once

#include <onyx/noncopyable.h>
#include <onyx/platform/window.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi {
class Window;
}

namespace onyx::rhi::vulkan {
class Instance;
class Surface : public NonCopyable {
  public:
    Surface( const Instance& instance, const platform::Window& window );
    ~Surface() override;

  private:
    const Instance& m_instance;
    VULKAN_HANDLE( VkSurfaceKHR, surface, nullptr );
};

} // namespace onyx::rhi::vulkan
