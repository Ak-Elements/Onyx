#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics
{
    struct ShaderDescriptorSet;

    namespace Vulkan
    {
        class Device;

        class DescriptorSetLayout : public NonCopyable
        {
        public:
            DescriptorSetLayout(const Device& device, onyxU8 set, const VkDescriptorSetLayoutCreateInfo& createInfo);
            DescriptorSetLayout(const Device& device, const ShaderDescriptorSet& descriptorSet);
            ~DescriptorSetLayout();

            onyxU8 GetSet() const { return m_Set; }
            const HashMap<String, VkWriteDescriptorSet>& GetWriteDescriptors() const { return m_WriteDescriptorSets; }

        private:
            const Device& m_Device;
            onyxU8 m_Set = static_cast<onyxU8>(INVALID_INDEX_8);

            VULKAN_HANDLE(VkDescriptorSetLayout, Layout, nullptr);
            HashMap<String, VkWriteDescriptorSet> m_WriteDescriptorSets;
        };
    }
}
