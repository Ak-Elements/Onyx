#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/commandpool.h>
#include <onyx/graphics/vulkan/commandbuffer.h>
#include <onyx/graphics/vulkan/graphicsapi.h>

namespace Onyx::Graphics
{
    struct Viewport;
    
    // TODO: add base for generic use
    // TODO: Change CommandBuffer returned to outside to RAII object that enqueues the buffer for submission
    // And internally the storage is just the 'raw' API command buffer without any functions which gets wrapped into the RAII object
    namespace Vulkan
    {
        class VulkanCommandBuffer;
        class VulkanGraphicsApi;

        class CommandBufferManager : public NonCopyable
        {
        public:
            static constexpr onyxU32 COMMAND_BUFFERS_PER_THREAD = 3;
            static constexpr onyxU32 SECONDARY_COMMAND_BUFFERS_PER_THREAD = 2;

            void Init(VulkanGraphicsApi& api, onyxU32 queueIndex, onyxU32 threadCount);
            void Shutdown();

            void Reset(Device& device, onyxU8 frameIndex);

            CommandBuffer& GetCommandBuffer(onyxU32 frameIndex, onyxU32 threadIndex, bool begin);
            UniquePtr<CommandBuffer> CreateSingleSubmitCommandBuffer(VulkanGraphicsApi& api, onyxU32 frameIndex, onyxU32 threadIndex);

        private:
            onyxU32 m_ThreadCount = 0;
            // graphic command pools 1 pool per thread per frame
            DynamicArray<CommandPool> m_CommandPools;
            
            DynamicArray<VulkanCommandBuffer> m_PrimaryBuffers;
            DynamicArray<VulkanCommandBuffer> m_SecondaryBuffers;

            DynamicArray<onyxU8> m_UsedBuffers;
            DynamicArray<onyxU8> m_UsedSecondaryBuffers;
        };
    }
}
