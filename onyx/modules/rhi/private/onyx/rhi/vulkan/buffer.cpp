#include <numeric>
#include <onyx/rhi/vulkan/buffer.h>
#include <onyx/rhi/vulkan/commandpool.h>
#include <onyx/rhi/vulkan/device.h>
#include <onyx/rhi/vulkan/memoryallocator.h>
#include <onyx/rhi/vulkan/commandbuffer.h>
#include <onyx/rhi/vulkan/vulkan.h>

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
            {
                DeviceMemory::Unmap();
            }
                
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

    void VulkanBuffer::Barrier(CommandBuffer& commandBuffer, Context newContext, Access newAccess)
    {
        Barrier(commandBuffer, newContext, newAccess, INVALID_INDEX_8);
    }

    void VulkanBuffer::Barrier(CommandBuffer& commandBuffer, Context newContext, Access newAccess, onyxS8 aliasIndex)
    {
        // TODO: hazard tracking for regions?
        VulkanCommandBuffer& vkCommandBuffer = static_cast<VulkanCommandBuffer&>(commandBuffer);

        Access currentAccess = m_Access;
        Context currentContext = m_Context;
        onyxU64 offset = 0;
        onyxU64 bufferSize = m_Properties.m_Size;
        if (aliasIndex != INVALID_INDEX_8)
        {
            AliasInfo& aliasInfo = m_Aliases[aliasIndex];
            currentAccess = aliasInfo.Access;
            currentContext = aliasInfo.Context;
            offset = aliasInfo.Offset;
            bufferSize = aliasInfo.Size;

            aliasInfo.Access = newAccess;
            aliasInfo.Context = newContext;
        }

        VkBufferMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2 };
        barrier.srcAccessMask = ToAccessFlag(currentAccess);
        barrier.srcStageMask = GetPipelineFlags(barrier.srcAccessMask, currentContext);
        barrier.dstAccessMask = ToAccessFlag(newAccess);
        barrier.dstStageMask = GetPipelineFlags(barrier.dstAccessMask, newContext);
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.buffer = m_Buffer;
        barrier.offset = offset;
        barrier.size = bufferSize;
        barrier.pNext = nullptr;

        VkDependencyInfo dependency_info{};
        dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency_info.bufferMemoryBarrierCount = 1;
        dependency_info.pBufferMemoryBarriers = &barrier;
        dependency_info.pNext = nullptr;

        vkCmdPipelineBarrier2(vkCommandBuffer.GetHandle(), &dependency_info);
        m_Access = newAccess;
        m_Context = newContext;
    }

    onyxS8 VulkanBuffer::Alias(const BufferProperties& properties)
    {
        // do we support device address?
        
        onyxU64 offset = 0;
        for (const AliasInfo& info : m_Aliases)
        {
            offset += info.Size;
        }

        m_Aliases.emplace_back(offset, properties.m_Size);

        //VkBufferCreateInfo createInfo{};
        //createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //createInfo.size = properties.m_Size;
        //createInfo.usage = GetUsageFlags();
        //// TODO: do we need to use concurrent?
        //createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //createInfo.pNext = nullptr;
        //
        //VkBuffer buffer = m_Allocator->Alias(m_Memory, createInfo, offset);
        //ONYX_UNUSED(buffer);
        //VkBufferDeviceAddressInfoKHR addressInfo{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR };
        //addressInfo.buffer = buffer;
        //onyxU64 gpuAddress = vkGetBufferDeviceAddress(m_Device->GetHandle(), &addressInfo);
        //
        //onyxU64 calculatedGpuAddress = m_GpuAddress + offset;
        //
        //ONYX_LOG_INFO("{}, {}", gpuAddress, calculatedGpuAddress);

        return static_cast<onyxS8>(m_Aliases.size() - 1);
    }

    void VulkanBuffer::Init(const void* data)
    {
        VkDevice device = m_Device->GetHandle();

        VkBufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = m_Properties.m_Size;
        createInfo.usage = GetUsageFlags();
        // TODO: do we need to use concurrent?
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.pNext = nullptr;

        VK_CHECK_RESULT(vkCreateBuffer(device, &createInfo, nullptr, &m_Buffer));
        SetResourceName(device, VK_OBJECT_TYPE_BUFFER, (onyxU64)m_Buffer, m_Properties.m_DebugName.empty() ? "Unnamed Buffer" : m_Properties.m_DebugName);

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);

        VkMemoryPropertyFlags requiredMemoryPropertyFlags, preferredMemoryPropertyFlags;
        GetMemoryPropertyFlags(m_Properties.m_CpuAccess, m_Properties.m_GpuAccess, requiredMemoryPropertyFlags, preferredMemoryPropertyFlags);

        m_Memory = m_Allocator->AllocateDedicatedMemory(m_Buffer, memoryRequirements.memoryTypeBits, requiredMemoryPropertyFlags, preferredMemoryPropertyFlags);
        m_IsNonCoherent = (requiredMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        m_Allocator->Bind(m_Buffer, m_Memory);

        if ( Enums::HasAllFlags(m_Properties.m_UsageFlags, BufferUsage::DeviceAddress ))
        {
            VkBufferDeviceAddressInfoKHR addressInfo{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR };
            addressInfo.buffer = m_Buffer;
            m_GpuAddress = vkGetBufferDeviceAddress(device, &addressInfo);
        }

        if ((m_Properties.m_CpuAccess == CPUAccess::UpdateUnsynchronized) || m_Properties.m_CpuAccess == CPUAccess::Write)
        {
            Map(MapMode::Write);
        }

        if (data != nullptr)
        {
            SetData(0, data, numeric_cast<onyxS32>(m_Properties.m_Size));
        }

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

        if (Enums::HasAnyFlags(properties.m_UsageFlags, BufferUsage::Vertex))
            usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_UsageFlags, BufferUsage::Index))
            usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_UsageFlags, BufferUsage::Uniform))
            usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_UsageFlags, BufferUsage::Storage))
            usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_UsageFlags, BufferUsage::Indirect))
            usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        if (Enums::HasAnyFlags(properties.m_UsageFlags, BufferUsage::DeviceAddress))
            usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        if (Enums::HasAnyFlags(properties.m_UsageFlags, BufferUsage::Conditional))
            usage |= VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT;

        if (properties.m_IsWritable)
            usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

        return usage;
    }
};
