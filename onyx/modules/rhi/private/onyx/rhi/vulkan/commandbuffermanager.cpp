#include <onyx/rhi/vulkan/commandbuffer.h>
#include <onyx/rhi/vulkan/commandbuffermanager.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/physicaldevice.h>

namespace onyx::rhi::vulkan {
void CommandBufferManager::init( VulkanGraphicsApi& api, uint32_t queueIndex, uint32_t threadCount ) {
    m_threadCount = threadCount;

    // TODO: Replace with a ring buffer per thread per queue ?
    uint32_t totalPoolCount = threadCount * MaxFramesInFlight;

    m_commandPools.reserve( totalPoolCount );
    m_primaryBuffers.reserve( totalPoolCount * CommandBuffersPerThread );
    m_secondaryBuffers.reserve( totalPoolCount * SecondaryCommandBuffersPerThread );

    for( uint32_t commandPoolIndex = 0; commandPoolIndex < totalPoolCount; ++commandPoolIndex ) {
        CommandPool& commandPool = m_commandPools.emplace_back( api.getDevice(),
                                                                queueIndex,
                                                                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

        uint8_t frameIndex = numericCast< uint8_t >( commandPoolIndex / threadCount );

#if !ONYX_IS_RETAIL
        commandPool.setFrameIndex( frameIndex );
#endif

        for( uint32_t i = 0; i < CommandBuffersPerThread; ++i )
            m_primaryBuffers.emplace_back( api,
                                           commandPool,
                                           VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                           frameIndex,
                                           format::format( "CommandBuffer {} {}", commandPoolIndex, i ) );

        for( uint32_t i = 0; i < SecondaryCommandBuffersPerThread; ++i )
            m_secondaryBuffers.emplace_back( api,
                                             commandPool,
                                             VK_COMMAND_BUFFER_LEVEL_SECONDARY,
                                             frameIndex,
                                             format::format( "SecondaryCommandBuffer {} {}", commandPoolIndex, i ) );
    }

    m_usedBuffers.resize( totalPoolCount, 0 );
    m_usedSecondaryBuffers.resize( totalPoolCount, 0 );
}

void CommandBufferManager::shutdown() {
    m_primaryBuffers.clear();
    m_secondaryBuffers.clear();
    m_commandPools.clear();
}

void CommandBufferManager::reset( Device& device, uint8_t frameIndex ) {
    for( uint32_t threadIndex = 0; threadIndex < m_threadCount; threadIndex++ ) {
        const uint32_t poolIndex = ( frameIndex * m_threadCount ) + threadIndex;
        ONYX_ASSERT( m_commandPools[ poolIndex ].getFrameIndex() == frameIndex );
        vkResetCommandPool( device.getHandle(), m_commandPools[ poolIndex ].getHandle(), 0 );

        m_usedBuffers[ poolIndex ] = 0;
        m_usedSecondaryBuffers[ poolIndex ] = 0;
    }
}

CommandBuffer& CommandBufferManager::getCommandBuffer( uint32_t frameIndex, uint32_t threadIndex, bool begin ) {
    const uint32_t commandPoolIndex = ( frameIndex * m_threadCount ) + threadIndex;
    const uint8_t currentUsedBufferIndex = m_usedBuffers[ commandPoolIndex ];

    ONYX_ASSERT( currentUsedBufferIndex < CommandBuffersPerThread );

    m_usedBuffers[ commandPoolIndex ] = currentUsedBufferIndex + (uint8_t)1;

    CommandBuffer&
        commandBuffer = m_primaryBuffers[ ( commandPoolIndex * CommandBuffersPerThread ) + currentUsedBufferIndex ];
    if( begin ) {
        commandBuffer.reset();
        commandBuffer.begin();
    }

    return commandBuffer;
}

UniquePtr< CommandBuffer > CommandBufferManager::createSingleSubmitCommandBuffer( VulkanGraphicsApi& api,
                                                                                  uint32_t frameIndex,
                                                                                  uint32_t threadIndex ) {
    const uint32_t commandPoolIndex = ( frameIndex * m_threadCount ) + threadIndex;
    UniquePtr< CommandBuffer > cmdBuffer = makeUnique< VulkanCommandBuffer >(
        api,
        m_commandPools[ commandPoolIndex ],
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        frameIndex,
        format::format( "Single Submit {} {}", commandPoolIndex, frameIndex ) );
    return cmdBuffer;
}
} // namespace onyx::rhi::vulkan
