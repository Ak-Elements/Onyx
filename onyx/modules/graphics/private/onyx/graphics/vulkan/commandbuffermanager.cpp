#include <onyx/graphics/vulkan/commandbuffer.h>
#include <onyx/graphics/vulkan/commandbuffermanager.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/physicaldevice.h>

namespace Onyx::Graphics::Vulkan
{
    void CommandBufferManager::Init(VulkanGraphicsApi& api, onyxU32 queueIndex, onyxU32 threadCount)
    {
        m_ThreadCount = threadCount;

        // TODO: Replace with a ring buffer per thread per queue ?
        onyxU32 totalPoolCount = threadCount * MAX_FRAMES_IN_FLIGHT;

        m_CommandPools.reserve(totalPoolCount);
        m_PrimaryBuffers.reserve(totalPoolCount * COMMAND_BUFFERS_PER_THREAD);
        m_SecondaryBuffers.reserve(totalPoolCount * SECONDARY_COMMAND_BUFFERS_PER_THREAD);

        for (onyxU32 commandPoolIndex = 0; commandPoolIndex < totalPoolCount; ++commandPoolIndex)
        {
            CommandPool& commandPool = m_CommandPools.emplace_back(api.GetDevice(), queueIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

            onyxU8 frameIndex = (commandPoolIndex % MAX_FRAMES_IN_FLIGHT);
            for (onyxU32 i = 0; i < COMMAND_BUFFERS_PER_THREAD; ++i)    
                m_PrimaryBuffers.emplace_back(api, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, frameIndex, Format::Format("CommandBuffer {} {}", commandPoolIndex, i));

            for (onyxU32 i = 0; i < SECONDARY_COMMAND_BUFFERS_PER_THREAD; ++i)
                m_SecondaryBuffers.emplace_back(api, commandPool, VK_COMMAND_BUFFER_LEVEL_SECONDARY, frameIndex, Format::Format("SecondaryCommandBuffer {} {}", commandPoolIndex, i));
        }

        m_UsedBuffers.resize(totalPoolCount, 0);
        m_UsedSecondaryBuffers.resize(totalPoolCount, 0);
    }

    void CommandBufferManager::Shutdown()
    {
        m_PrimaryBuffers.clear();
        m_SecondaryBuffers.clear();
        m_CommandPools.clear();
    }

    void CommandBufferManager::Reset(Device& device, onyxU8 frameIndex)
    {
        for (onyxU32 threadIndex = 0; threadIndex < m_ThreadCount; threadIndex++)
        {
            const onyxU32 poolIndex = (frameIndex * m_ThreadCount) + threadIndex;
            vkResetCommandPool(device.GetHandle(), m_CommandPools[poolIndex].GetHandle(), 0);

            m_UsedBuffers[poolIndex] = 0;
            m_UsedSecondaryBuffers[poolIndex] = 0;
        }
    }

    CommandBuffer& CommandBufferManager::GetCommandBuffer(onyxU32 frameIndex, onyxU32 threadIndex, bool begin)
    {
        const onyxU32 commandPoolIndex = (frameIndex * m_ThreadCount) + threadIndex;
        const onyxU8 currentUsedBufferIndex = m_UsedBuffers[commandPoolIndex];

        ONYX_ASSERT(currentUsedBufferIndex < COMMAND_BUFFERS_PER_THREAD);
        
        m_UsedBuffers[commandPoolIndex] = currentUsedBufferIndex + (onyxU8)1;

        CommandBuffer& commandBuffer = m_PrimaryBuffers[(commandPoolIndex * COMMAND_BUFFERS_PER_THREAD) + currentUsedBufferIndex];
        if (begin)
        {
            commandBuffer.Reset();
            commandBuffer.Begin();

            // TODO: reset query pools
            /*
             GpuThreadFramePools * thread_pools = cb->thread_frame_pool;
             thread_pools->time_queries->reset();
             vkCmdResetQueryPool(cb->vk_command_buffer, thread_pools->vulkan_timestamp_query_pool, 0, thread_pools->time_queries->time_queries.size);

             // Pipeline statistics
             vkCmdResetQueryPool(cb->vk_command_buffer, thread_pools->vulkan_pipeline_stats_query_pool, 0, GpuPipelineStatistics::Count);

             vkCmdBeginQuery(cb->vk_command_buffer, thread_pools->vulkan_pipeline_stats_query_pool, 0, 0);
             */

        }

        return commandBuffer;
    }

    UniquePtr<CommandBuffer> CommandBufferManager::CreateSingleSubmitCommandBuffer(VulkanGraphicsApi& api, onyxU32 frameIndex, onyxU32 threadIndex)
    {
        const onyxU32 commandPoolIndex = (frameIndex * m_ThreadCount) + threadIndex;
        UniquePtr<CommandBuffer> cmdBuffer = MakeUnique<VulkanCommandBuffer>(api, m_CommandPools[commandPoolIndex], VK_COMMAND_BUFFER_LEVEL_PRIMARY, frameIndex, Format::Format("Single Submit {} {}", commandPoolIndex, frameIndex));
        return cmdBuffer;
    }
}
