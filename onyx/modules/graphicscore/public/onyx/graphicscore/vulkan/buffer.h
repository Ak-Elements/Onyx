#pragma once

#include <onyx/graphicscore/buffer.h>
#include <onyx/graphicscore/vulkan/devicememory.h>
#include <onyx/graphicscore/vulkan/vulkan.h>

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

        void Destroy();

        void* Map(MapMode mode) override;
        void Unmap() override;

        void Flush(onyxU32 offset, onyxU32 count) override;
        void SetData(onyxS32 offset, const void* data, onyxS32 length) override;

        const VkDescriptorBufferInfo& GetDescriptorInfo() const { return m_DescriptorInfo; }

        onyxU64 GetAliasOffset(onyxS8 alias) const override
        {
            if (alias == INVALID_INDEX_8)
                return 0;
            return m_Aliases[alias].Offset;
        }

        onyxU64 GetAliasSize(onyxS8 alias) const override
        {
            if (alias == INVALID_INDEX_8)
                return m_Properties.m_Size;

            return m_Aliases[alias].Size;
        }

        void ClearAliases() override { m_Aliases.clear(); }

        void Barrier(CommandBuffer& commandBuffer, Context newContext, Access newAccess) override;
        void Barrier(CommandBuffer& commandBuffer, Context newContext, Access newAccess, onyxS8 aliasIndex) override;
        onyxS8 Alias(const BufferProperties& properties) override;

    private:
        void Init(const void* data = nullptr);
        void UpdateDescriptorInfo();

        VkBufferUsageFlags GetUsageFlags() const;
        static VkBufferUsageFlags GetUsageFlags(const BufferProperties& properties);

        struct AliasInfo
        {
            onyxU64 Offset;
            onyxU64 Size;

            Access Access = Access::None;
            Context Context = Context::Graphics;

            VkBuffer m_Buffer = nullptr;//only used if device address is not supported
        };

        DynamicArray<AliasInfo> m_Aliases;

    private:
        const Device* m_Device;
        VULKAN_HANDLE(VkBuffer, Buffer, nullptr);
        VkDescriptorBufferInfo m_DescriptorInfo;

        Access m_Access = Access::None;
        Context m_Context = Context::Graphics;
    };
}
