#pragma once

#include <onyx/graphics/descriptorset.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics
{
    class Buffer;
    struct TextureHandle;
}
namespace Onyx::Graphics::Vulkan
{
    
    class DescriptorPool;
    class DescriptorSetLayout;
    class Device;
    class Shader;

    class DescriptorSet : public Graphics::DescriptorSet
    {
    public:
        DescriptorSet(const Device& device, onyxU8 set, VkDescriptorSetAllocateInfo allocateInfo);
        DescriptorSet(const Device& device, const DescriptorPool& pool, const DescriptorSetLayout& descriptorSetLayout);
        DescriptorSet(const Device& device, const DescriptorPool& pool, const DescriptorSetLayout& descriptorSetLayout, const StringView& debugName);
        // Don't need to be cleaned up as they will be cleaned up when the pool is destroyed
        //vkFreeDescriptorSets()
        ~DescriptorSet() override = default;

        bool HasPendingUpdates() const override { return m_PendingDescriptorUpdates.empty() == false; }

        void UpdateDescriptors() override;

        void Bind(const Reference<Buffer>& bufferHandle, const String& bindingName) override;
        void Bind(const TextureHandle& textureHandle, const String& bindingName) override;

        HashSet<String> GetBindingIds() const override;

    private:
        const Device& m_Device;

        VULKAN_HANDLE(VkDescriptorSet, DescriptorSet, nullptr);

        DynamicArray<VkWriteDescriptorSet> m_PendingDescriptorUpdates;
        DynamicArray<VkImageMemoryBarrier2KHR> m_PendingBarriers;

        HashMap<String, VkWriteDescriptorSet> m_WriteDescriptorSets;
    };
}
