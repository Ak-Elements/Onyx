#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/commandbuffer.h>
#include <onyx/rhi/vulkan/commandpool.h>
#include <onyx/rhi/vulkan/graphicsapi.h>

namespace onyx::rhi {
struct Viewport;

// TODO: add base for generic use
// TODO: Change CommandBuffer returned to outside to RAII object that enqueues the buffer for submission
// And internally the storage is just the 'raw' API command buffer without any functions which gets wrapped into the
// RAII object
namespace vulkan {
class VulkanCommandBuffer;
class VulkanGraphicsApi;

class CommandBufferManager : public NonCopyable {
  public:
    static constexpr uint32_t CommandBuffersPerThread = 5;
    static constexpr uint32_t SecondaryCommandBuffersPerThread = 2;

    void init( VulkanGraphicsApi& api, uint32_t queueIndex, uint32_t threadCount );
    void shutdown();

    void reset( Device& device, uint8_t frameIndex );

    CommandBuffer& getCommandBuffer( uint32_t frameIndex, uint32_t threadIndex, bool begin );
    UniquePtr< CommandBuffer > createSingleSubmitCommandBuffer( VulkanGraphicsApi& api,
                                                                uint32_t frameIndex,
                                                                uint32_t threadIndex );

  private:
    uint32_t m_threadCount = 0;
    // graphic command pools 1 pool per thread per frame
    DynamicArray< CommandPool > m_commandPools;

    DynamicArray< VulkanCommandBuffer > m_primaryBuffers;
    DynamicArray< VulkanCommandBuffer > m_secondaryBuffers;

    DynamicArray< uint8_t > m_usedBuffers;
    DynamicArray< uint8_t > m_usedSecondaryBuffers;
};
} // namespace vulkan
} // namespace onyx::rhi
