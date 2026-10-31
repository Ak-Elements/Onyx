#include <onyx/rhi/vulkan/buffer.h>
#include <onyx/rhi/vulkan/commandbuffer.h>
#include <onyx/rhi/vulkan/commandpool.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/memoryallocator.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
VulkanBuffer::VulkanBuffer( VulkanGraphicsApi& api, const BufferProperties& properties )
    : rhi::Buffer( properties )
    , DeviceMemory( api.getAllocator() )
    , m_device( &api.getDevice() ) {
    init();
}

VulkanBuffer::~VulkanBuffer() {
    if( m_buffer != nullptr ) {
        if( m_MappedPtr ) {
            DeviceMemory::Unmap();
        }

        Destroy();
    }
}

void VulkanBuffer::Destroy() {
    if( m_buffer != nullptr ) {
        vkDestroyBuffer( m_device->getHandle(), m_buffer, nullptr );
        m_buffer = nullptr;
    }
}

void VulkanBuffer::barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess ) {
    barrier( commandBuffer, newContext, newAccess, InvalidIndex8 );
}

void VulkanBuffer::barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess, int8_t aliasIndex ) {
    // TODO: hazard tracking for regions?
    VulkanCommandBuffer& vkCommandBuffer = static_cast< VulkanCommandBuffer& >( commandBuffer );

    Access currentAccess = m_access;
    Context currentContext = m_context;
    uint64_t offset = 0;
    uint64_t bufferSize = m_properties.m_Size;
    if( aliasIndex != InvalidIndex8 ) {
        AliasInfo& aliasInfo = m_aliases[ aliasIndex ];
        currentAccess = aliasInfo.Access;
        currentContext = aliasInfo.Context;
        offset = aliasInfo.Offset;
        bufferSize = aliasInfo.Size;

        aliasInfo.Access = newAccess;
        aliasInfo.Context = newContext;
    }

    VkBufferMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2 };
    barrier.srcAccessMask = toAccessFlag( currentAccess );
    barrier.srcStageMask = getPipelineFlags( barrier.srcAccessMask, currentContext );
    barrier.dstAccessMask = toAccessFlag( newAccess );
    barrier.dstStageMask = getPipelineFlags( barrier.dstAccessMask, newContext );
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.buffer = m_buffer;
    barrier.offset = offset;
    barrier.size = bufferSize;
    barrier.pNext = nullptr;

    VkDependencyInfo dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.bufferMemoryBarrierCount = 1;
    dependencyInfo.pBufferMemoryBarriers = &barrier;
    dependencyInfo.pNext = nullptr;

    vkCmdPipelineBarrier2( vkCommandBuffer.getHandle(), &dependencyInfo );
    m_access = newAccess;
    m_context = newContext;
}

int8_t VulkanBuffer::alias( const BufferProperties& properties ) {
    // do we support device address?

    uint64_t offset = 0;
    for( const AliasInfo& info : m_aliases ) {
        offset += info.Size;
    }

    m_aliases.emplace_back( offset, properties.m_Size );

    // VkBufferCreateInfo createInfo{};
    // createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // createInfo.size = properties.m_Size;
    // createInfo.usage = GetUsageFlags();
    //// TODO: do we need to use concurrent?
    // createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // createInfo.pNext = nullptr;
    //
    // VkBuffer buffer = m_Allocator->Alias(m_Memory, createInfo, offset);
    // ONYX_UNUSED(buffer);
    // VkBufferDeviceAddressInfoKHR addressInfo{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR };
    // addressInfo.buffer = buffer;
    // uint64_t gpuAddress = vkGetBufferDeviceAddress(m_Device->GetHandle(), &addressInfo);
    //
    // uint64_t calculatedGpuAddress = m_GpuAddress + offset;
    //
    // ONYX_LOG_INFO("{}, {}", gpuAddress, calculatedGpuAddress);

    return static_cast< int8_t >( m_aliases.size() - 1 );
}

void VulkanBuffer::init( const void* data ) {
    VkDevice device = m_device->getHandle();

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = m_properties.m_Size;
    createInfo.usage = getUsageFlags();
    // TODO: do we need to use concurrent?
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.pNext = nullptr;

    VK_CHECK_RESULT( vkCreateBuffer( device, &createInfo, nullptr, &m_buffer ) );
    setResourceName( device,
                     VK_OBJECT_TYPE_BUFFER,
                     (uint64_t)m_buffer,
                     m_properties.m_DebugName.empty() ? "Unnamed Buffer" : m_properties.m_DebugName );

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements( device, m_buffer, &memoryRequirements );

    VkMemoryPropertyFlags requiredMemoryPropertyFlags, preferredMemoryPropertyFlags;
    GetMemoryPropertyFlags( m_properties.m_CpuAccess,
                            m_properties.m_GpuAccess,
                            requiredMemoryPropertyFlags,
                            preferredMemoryPropertyFlags );

    m_Memory = m_Allocator->allocateDedicatedMemory( m_buffer,
                                                     memoryRequirements.memoryTypeBits,
                                                     requiredMemoryPropertyFlags,
                                                     preferredMemoryPropertyFlags );
    m_IsNonCoherent = ( requiredMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) ==
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    m_Allocator->bind( m_buffer, m_Memory );

    if( enums::all( m_properties.m_UsageFlags, BufferUsage::DeviceAddress ) ) {
        VkBufferDeviceAddressInfoKHR addressInfo{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR };
        addressInfo.buffer = m_buffer;
        m_gpuAddress = vkGetBufferDeviceAddress( device, &addressInfo );
    }

    if( ( m_properties.m_CpuAccess == CPUAccess::UpdateUnsynchronized ) ||
        m_properties.m_CpuAccess == CPUAccess::Write ) {
        map( MapMode::Write );
    }

    if( data != nullptr ) {
        setData( 0, data, numericCast< int32_t >( m_properties.m_Size ) );
    }

    updateDescriptorInfo();
}

void VulkanBuffer::updateDescriptorInfo() {
    m_descriptorInfo.buffer = m_buffer;
    m_descriptorInfo.offset = 0;
    m_descriptorInfo.range = m_properties.m_Size;
}

void* VulkanBuffer::map( MapMode mode ) {
    m_dataPointer = DeviceMemory::Map( mode );
    return m_dataPointer;
}

void VulkanBuffer::unmap() {
    DeviceMemory::Unmap();
}

void VulkanBuffer::flush( uint32_t /*offset*/, uint32_t /*count*/ ) {
    if( m_IsNonCoherent == false )
        return;

    ONYX_ASSERT( false, "Not implemented" );
}

void VulkanBuffer::setData( uint64_t offset, const void* data, uint64_t length ) {
    std::memcpy( &static_cast< uint8_t* >( m_dataPointer )[ offset ], data, length );
}

VkBufferUsageFlags VulkanBuffer::getUsageFlags() const {
    return getUsageFlags( m_properties );
}

/*static*/ VkBufferUsageFlags VulkanBuffer::getUsageFlags( const BufferProperties& properties ) {
    VkBufferUsageFlags usage = 0;

    if( properties.m_GpuAccess == GPUAccess::Staging && properties.m_CpuAccess == CPUAccess::Write )
        usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    else
        usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if( enums::any( properties.m_UsageFlags, BufferUsage::Vertex ) )
        usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if( enums::any( properties.m_UsageFlags, BufferUsage::Index ) )
        usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if( enums::any( properties.m_UsageFlags, BufferUsage::Uniform ) )
        usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if( enums::any( properties.m_UsageFlags, BufferUsage::Storage ) )
        usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if( enums::any( properties.m_UsageFlags, BufferUsage::Indirect ) )
        usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if( enums::any( properties.m_UsageFlags, BufferUsage::DeviceAddress ) )
        usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    if( enums::any( properties.m_UsageFlags, BufferUsage::Conditional ) )
        usage |= VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT;

    if( properties.m_IsWritable )
        usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                 VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

    return usage;
}
}; // namespace onyx::rhi::vulkan
