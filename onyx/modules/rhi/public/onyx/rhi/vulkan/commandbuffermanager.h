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
    static constexpr uint32_t COMMAND_BUFFERS_PER_THREAD = 5;
    static constexpr uint32_t SECONDARY_COMMAND_BUFFERS_PER_THREAD = 2;

    void Init( VulkanGraphicsApi& api, uint32_t queueIndex, uint32_t threadCount );
    void Shutdown();

    void Reset( Device& device, uint8_t frameIndex );

    CommandBuffer& GetCommandBuffer( uint32_t frameIndex, uint32_t threadIndex, bool begin );
    UniquePtr< CommandBuffer > CreateSingleSubmitCommandBuffer( VulkanGraphicsApi& api,
                                                                uint32_t frameIndex,
                                                                uint32_t threadIndex );

  private:
    uint32_t m_ThreadCount = 0;
    // graphic command pools 1 pool per thread per frame
    DynamicArray< CommandPool > m_CommandPools;

    DynamicArray< VulkanCommandBuffer > m_PrimaryBuffers;
    DynamicArray< VulkanCommandBuffer > m_SecondaryBuffers;

    DynamicArray< uint8_t > m_UsedBuffers;
    DynamicArray< uint8_t > m_UsedSecondaryBuffers;
};
} // namespace vulkan
} // namespace onyx::rhi
