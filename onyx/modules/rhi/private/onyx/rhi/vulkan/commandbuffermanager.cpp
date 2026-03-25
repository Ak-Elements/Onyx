#include <onyx/rhi/vulkan/commandbuffer.h>
#include <onyx/rhi/vulkan/commandbuffermanager.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/physicaldevice.h>

namespace onyx::rhi::vulkan {
void CommandBufferManager::Init( VulkanGraphicsApi& api, uint32_t queueIndex, uint32_t threadCount ) {
    m_ThreadCount = threadCount;

    // TODO: Replace with a ring buffer per thread per queue ?
    uint32_t totalPoolCount = threadCount * MAX_FRAMES_IN_FLIGHT;

    m_CommandPools.reserve( totalPoolCount );
    m_PrimaryBuffers.reserve( totalPoolCount * COMMAND_BUFFERS_PER_THREAD );
    m_SecondaryBuffers.reserve( totalPoolCount * SECONDARY_COMMAND_BUFFERS_PER_THREAD );

    for ( uint32_t commandPoolIndex = 0; commandPoolIndex < totalPoolCount; ++commandPoolIndex ) {
        CommandPool& commandPool = m_CommandPools.emplace_back( api.GetDevice(),
                                                                queueIndex,
                                                                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

        uint8_t frameIndex = ( commandPoolIndex % MAX_FRAMES_IN_FLIGHT );
        for ( uint32_t i = 0; i < COMMAND_BUFFERS_PER_THREAD; ++i )
            m_PrimaryBuffers.emplace_back( api,
                                           commandPool,
                                           VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                           frameIndex,
                                           format::format( "CommandBuffer {} {}", commandPoolIndex, i ) );

        for ( uint32_t i = 0; i < SECONDARY_COMMAND_BUFFERS_PER_THREAD; ++i )
            m_SecondaryBuffers.emplace_back( api,
                                             commandPool,
                                             VK_COMMAND_BUFFER_LEVEL_SECONDARY,
                                             frameIndex,
                                             format::format( "SecondaryCommandBuffer {} {}", commandPoolIndex, i ) );
    }

    m_UsedBuffers.resize( totalPoolCount, 0 );
    m_UsedSecondaryBuffers.resize( totalPoolCount, 0 );
}

void CommandBufferManager::Shutdown() {
    m_PrimaryBuffers.clear();
    m_SecondaryBuffers.clear();
    m_CommandPools.clear();
}

void CommandBufferManager::Reset( Device& device, uint8_t frameIndex ) {
    for ( uint32_t threadIndex = 0; threadIndex < m_ThreadCount; threadIndex++ ) {
        const uint32_t poolIndex = ( frameIndex * m_ThreadCount ) + threadIndex;
        vkResetCommandPool( device.GetHandle(), m_CommandPools[ poolIndex ].GetHandle(), 0 );

        m_UsedBuffers[ poolIndex ] = 0;
        m_UsedSecondaryBuffers[ poolIndex ] = 0;
    }
}

CommandBuffer& CommandBufferManager::GetCommandBuffer( uint32_t frameIndex, uint32_t threadIndex, bool begin ) {
    const uint32_t commandPoolIndex = ( frameIndex * m_ThreadCount ) + threadIndex;
    const uint8_t currentUsedBufferIndex = m_UsedBuffers[ commandPoolIndex ];

    ONYX_ASSERT( currentUsedBufferIndex < COMMAND_BUFFERS_PER_THREAD );

    m_UsedBuffers[ commandPoolIndex ] = currentUsedBufferIndex + (uint8_t)1;

    CommandBuffer&
        commandBuffer = m_PrimaryBuffers[ ( commandPoolIndex * COMMAND_BUFFERS_PER_THREAD ) + currentUsedBufferIndex ];
    if ( begin ) {
        commandBuffer.Reset();
        commandBuffer.Begin();

        // TODO: reset query pools
        /*
         GpuThreadFramePools * thread_pools = cb->thread_frame_pool;
         thread_pools->time_queries->reset();
         vkCmdResetQueryPool(cb->vk_command_buffer, thread_pools->vulkan_timestamp_query_pool, 0,
         thread_pools->time_queries->time_queries.size);

         // Pipeline statistics
         vkCmdResetQueryPool(cb->vk_command_buffer, thread_pools->vulkan_pipeline_stats_query_pool, 0,
         GpuPipelineStatistics::Count);

         vkCmdBeginQuery(cb->vk_command_buffer, thread_pools->vulkan_pipeline_stats_query_pool, 0, 0);
         */
    }

    return commandBuffer;
}

UniquePtr< CommandBuffer > CommandBufferManager::CreateSingleSubmitCommandBuffer( VulkanGraphicsApi& api,
                                                                                  uint32_t frameIndex,
                                                                                  uint32_t threadIndex ) {
    const uint32_t commandPoolIndex = ( frameIndex * m_ThreadCount ) + threadIndex;
    UniquePtr< CommandBuffer > cmdBuffer = makeUnique< VulkanCommandBuffer >(
        api,
        m_CommandPools[ commandPoolIndex ],
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        frameIndex,
        format::format( "Single Submit {} {}", commandPoolIndex, frameIndex ) );
    return cmdBuffer;
}
} // namespace onyx::rhi::vulkan
