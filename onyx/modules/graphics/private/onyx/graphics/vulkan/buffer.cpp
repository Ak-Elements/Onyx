#include <onyx/graphics/vulkan/buffer.h>
#include <onyx/graphics/vulkan/commandpool.h>
#include <onyx/graphics/vulkan/device.h>
#include <onyx/graphics/vulkan/memoryallocator.h>
#include <onyx/graphics/vulkan/commandbuffer.h>

namespace Onyx::Graphics::Vulkan
{
    VulkanBuffer::VulkanBuffer(VulkanGraphicsApi& api, const BufferProperties& properties)
        : Graphics::Buffer(properties)
        , DeviceMemory(api.GetAllocator())
        , m_Device(&api.GetDevice())
    {
        Init();
    }

    VulkanBuffer::~VulkanBuffer()
    {
        if (m_Buffer != nullptr)
        {
            if (m_MappedPtr)
                DeviceMemory::Unmap();
            Destroy();
        }
    }

    void VulkanBuffer::Destroy()
    {
        if (m_Buffer != nullptr)
        {
            vkDestroyBuffer(m_Device->GetHandle(), m_Buffer, nullptr);
            m_Buffer = nullptr;
        }
    }

    void VulkanBuffer::Transition(VulkanCommandBuffer& commandBuffer, VkAccessFlags2 srcFlags, VkAccessFlags2 dstFlags)
    {
        VkBufferMemoryBarrier2 barrier;
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
        barrier.srcAccessMask = srcFlags;
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
        barrier.dstAccessMask = dstFlags;
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
        barrier.buffer = m_Buffer;
        barrier.offset = 0;
        barrier.size = m_Properties.m_Size;
        barrier.pNext = nullptr;

        VkDependencyInfo dependency_info;
        dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency_info.bufferMemoryBarrierCount = 1;
        dependency_info.pBufferMemoryBarriers = &barrier;
        dependency_info.pNext = nullptr;

        vkCmdPipelineBarrier2(commandBuffer.GetHandle(), &dependency_info);
    }

    void VulkanBuffer::Init(const void* data)
    {
        VkBufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = m_Properties.m_Size;
        createInfo.usage = GetUsageFlags();
        // TODO: do we need to use concurrent?
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.pNext = nullptr;

        VK_CHECK_RESULT(vkCreateBuffer(m_Device->GetHandle(), &createInfo, nullptr, &m_Buffer))
        SetResourceName(m_Device->GetHandle(), VK_OBJECT_TYPE_BUFFER, (onyxU64)m_Buffer, m_Properties.m_DebugName.empty() ? "Unnamed Buffer" : m_Properties.m_DebugName);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_Device->GetHandle(), m_Buffer, &memoryRequirements);

        VkMemoryPropertyFlags requiredMemoryPropertyFlags, preferredMemoryPropertyFlags;
        GetMemoryPropertyFlags(m_Properties.m_CpuAccess, m_Properties.m_GpuAccess, requiredMemoryPropertyFlags, preferredMemoryPropertyFlags);

        //TODO:
        m_Memory = m_Allocator->AllocateDedicatedMemory(m_Buffer, memoryRequirements.memoryTypeBits, requiredMemoryPropertyFlags, preferredMemoryPropertyFlags);
        m_IsNonCoherent = (requiredMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        m_Allocator->Bind(m_Buffer, m_Memory);

        if ((m_Properties.m_CpuAccess == CPUAccess::UpdateUnsynchronized) || m_Properties.m_CpuAccess == CPUAccess::Write)
            Map(MapMode::Write);

        if (data != nullptr)
            SetData(0, data, m_Properties.m_Size);

        UpdateDescriptorInfo();

    }

    void VulkanBuffer::UpdateDescriptorInfo()
    {
        m_DescriptorInfo.buffer = m_Buffer;
        m_DescriptorInfo.offset = 0;
        m_DescriptorInfo.range = m_Properties.m_Size;
    }

    void* VulkanBuffer::Map(MapMode mode)
    {
        m_DataPointer = DeviceMemory::Map(mode);
        return m_DataPointer;
    }

    void VulkanBuffer::Unmap()
    {
        DeviceMemory::Unmap();
    }

    void VulkanBuffer::Flush(onyxU32 /*offset*/, onyxU32 /*count*/)
    {
        if (m_IsNonCoherent == false)
            return;

        ONYX_ASSERT(false, "Not implemented");
    }

    void VulkanBuffer::SetData(onyxS32 offset, const void* data, onyxS32 length)
    {
        std::memcpy(&static_cast<char*>(m_DataPointer)[offset], data, length);
    }

    void VulkanBuffer::Copy(CommandPool& commandPool, const VulkanBuffer& src, VkDeviceSize size)
    {
        ONYX_UNUSED(commandPool);
        ONYX_UNUSED(src);
        ONYX_UNUSED(size);
;        // TODO: reimplement
        /*commandPool.SingleSubmitBuffer([&](VkCommandBuffer commandBuffer)
        {
            VkBufferCopy copyRegion;
            copyRegion.srcOffset = 0; // Optional
            copyRegion.dstOffset = 0; // Optional
            copyRegion.size = size;

            vkCmdCopyBuffer(commandBuffer, src.GetHandle(), GetHandle(), 1, &copyRegion);
        });*/
    }

    VkBufferUsageFlags VulkanBuffer::GetUsageFlags() const
    {
        return GetUsageFlags(m_Properties);
    }

    /*static*/ VkBufferUsageFlags VulkanBuffer::GetUsageFlags(const BufferProperties& properties)
    {
        VkBufferUsageFlags usage = 0;

        if (properties.m_GpuAccess == GPUAccess::Staging && properties.m_CpuAccess == CPUAccess::Write)
            usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        else
            usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        if (Enums::HasAnyFlags(properties.m_BindFlags, BufferType::Vertex))
            usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_BindFlags, BufferType::Index))
            usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_BindFlags, BufferType::Uniform))
            usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_BindFlags, BufferType::Buffer))
            usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_BindFlags, BufferType::Indirect))
            usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

        if (properties.m_IsWritable)
            usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

        return usage;
    }
};
