#pragma once

#include <onyx/graphics/buffer.h>
#include <onyx/graphics/vulkan/devicememory.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class VulkanCommandBuffer;
    class CommandPool;
    class Device;
    class VulkanGraphicsApi;

    class VulkanBuffer : public Buffer, public DeviceMemory
    {
    public:
        VulkanBuffer(VulkanGraphicsApi& api, const BufferProperties& properties);
        ~VulkanBuffer() override;

        void Copy(CommandPool& commandPool, const VulkanBuffer& src, VkDeviceSize size);
        void Destroy();

        void* Map(MapMode mode) override;
        void Unmap() override;

        void Flush(onyxU32 offset, onyxU32 count) override;
        void SetData(onyxS32 offset, const void* data, onyxS32 length) override;

        const VkDescriptorBufferInfo& GetDescriptorInfo() const { return m_DescriptorInfo; }

        void Transition(VulkanCommandBuffer& commandBuffer, VkAccessFlags2 srcFlags, VkAccessFlags2 dstFlags);

    private:
        void Init(const void* data = nullptr);
        void UpdateDescriptorInfo();

        VkBufferUsageFlags GetUsageFlags() const;
        static VkBufferUsageFlags GetUsageFlags(const BufferProperties& properties);

    private:
        const Device* m_Device;
        VULKAN_HANDLE(VkBuffer, Buffer, nullptr);
        VkDescriptorBufferInfo m_DescriptorInfo;
    };
}
