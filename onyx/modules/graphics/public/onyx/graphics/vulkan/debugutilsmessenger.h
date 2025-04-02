#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx
{
    class Engine;
    class Logger;

    namespace Graphics::Vulkan
    {
        class Instance;

        class DebugUtilsMessenger : public NonCopyable
        {
        public:
            DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold);
            ~DebugUtilsMessenger();

            VkDebugUtilsMessageSeverityFlagBitsEXT GetSeverity() const { return m_Severity; }

            static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
                const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                const VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* const pCallbackData,
                void* const pUserData);

        private:
            const Instance& m_Instance;
            const VkDebugUtilsMessageSeverityFlagBitsEXT m_Severity;

            VULKAN_HANDLE(VkDebugUtilsMessengerEXT, Messenger, nullptr);
        };
    }
}
