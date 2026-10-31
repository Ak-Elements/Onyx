#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class Device;

class Semaphore : public NonCopyable {
  public:
    explicit Semaphore( const Device& device );
    explicit Semaphore( const Device& device, bool isTimelineSemaphore );
    Semaphore( Semaphore&& other ) noexcept;
    ~Semaphore() override;

  private:
    const Device& m_device;
    VULKAN_HANDLE( VkSemaphore, semaphore, nullptr )
};
} // namespace onyx::rhi::vulkan
