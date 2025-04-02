#include <onyx/graphics/vulkan/debugutilsmessenger.h>

#include <onyx/graphics/vulkan/instance.h>
#include <vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    namespace
    {
        const char* ObjectTypeToString(const VkObjectType objectType)
        {
            switch (objectType)
            {
#define STR(e) case VK_OBJECT_TYPE_ ## e: return # e
            STR(UNKNOWN);
            STR(INSTANCE);
            STR(PHYSICAL_DEVICE);
            STR(DEVICE);
            STR(QUEUE);
            STR(SEMAPHORE);
            STR(COMMAND_BUFFER);
            STR(FENCE);
            STR(DEVICE_MEMORY);
            STR(BUFFER);
            STR(IMAGE);
            STR(EVENT);
            STR(QUERY_POOL);
            STR(BUFFER_VIEW);
            STR(IMAGE_VIEW);
            STR(SHADER_MODULE);
            STR(PIPELINE_CACHE);
            STR(PIPELINE_LAYOUT);
            STR(RENDER_PASS);
            STR(PIPELINE);
            STR(DESCRIPTOR_SET_LAYOUT);
            STR(SAMPLER);
            STR(DESCRIPTOR_POOL);
            STR(DESCRIPTOR_SET);
            STR(FRAMEBUFFER);
            STR(COMMAND_POOL);
            STR(SAMPLER_YCBCR_CONVERSION);
            STR(DESCRIPTOR_UPDATE_TEMPLATE);
            STR(SURFACE_KHR);
            STR(SWAPCHAIN_KHR);
            STR(DISPLAY_KHR);
            STR(DISPLAY_MODE_KHR);
            STR(DEBUG_REPORT_CALLBACK_EXT);
            STR(DEBUG_UTILS_MESSENGER_EXT);
            STR(ACCELERATION_STRUCTURE_KHR);
            STR(VALIDATION_CACHE_EXT);
            STR(PERFORMANCE_CONFIGURATION_INTEL);
            STR(DEFERRED_OPERATION_KHR);
            STR(INDIRECT_COMMANDS_LAYOUT_NV);
#undef STR
            default: return "unknown";
            }
        }

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                              const VkAllocationCallbacks* pAllocator,
                                              VkDebugUtilsMessengerEXT* pCallback)
        {
            const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
                instance, "vkCreateDebugUtilsMessengerEXT"));
            return func != nullptr
                       ? func(instance, pCreateInfo, pAllocator, pCallback)
                       : VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback,
                                           const VkAllocationCallbacks* pAllocator)
        {
            const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
                instance, "vkDestroyDebugUtilsMessengerEXT"));
            if (func != nullptr)
            {
                func(instance, callback, pAllocator);
            }
        }
    }

    DebugUtilsMessenger::DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT severity)
        : m_Instance(instance)
        , m_Severity(severity)
    {
        if (m_Instance.GetValidationsEnabled() == false)
        {
            return;
        }
        
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.flags = 0;
        createInfo.messageSeverity = severity;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugUtilsMessenger::VulkanDebugCallback;
        createInfo.pUserData = nullptr;
        createInfo.pNext = nullptr;

        VK_CHECK_RESULT(CreateDebugUtilsMessengerEXT(m_Instance.GetHandle(), &createInfo, nullptr, &m_Messenger));
    }

    DebugUtilsMessenger::~DebugUtilsMessenger()
    {
        if (m_Messenger != nullptr)
        {
            DestroyDebugUtilsMessengerEXT(m_Instance.GetHandle(), m_Messenger, nullptr);
            m_Messenger = nullptr;
        }
    }

    VkBool32 DebugUtilsMessenger::VulkanDebugCallback(
        const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        const VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* const pCallbackData,
        void* const /*userData*/)
    {
        LogLevel logLevel;
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            logLevel = LogLevel::Trace;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            logLevel = LogLevel::Information;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            logLevel = LogLevel::Warning;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            logLevel = LogLevel::Error;
            break;
        default:
            logLevel = LogLevel::Fatal;
        }

        std::string_view messageTypeString;
        switch (messageType)
        {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            messageTypeString = "GENERAL: ";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            messageTypeString = "VALIDATION: ";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            messageTypeString = "PERFORMANCE: ";
            break;
        default:
            messageTypeString = "UNKNOWN: ";
        }

        const char* errorMessage = Format::Format("{} {}", messageTypeString, pCallbackData->pMessage);
        Logger::s_DefaultLogger->Log(logLevel, errorMessage);

        const char* objectInfoMessage;
        constexpr std::string_view fmtString = " - Object [ {} ]: \n Type: {} \n Handle: {} \n Name: {} \n ";
        if (pCallbackData->objectCount > 0 && messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            for (onyxU32 i = 0; i != pCallbackData->objectCount; ++i)
            {
                const auto object = pCallbackData->pObjects[i];
                objectInfoMessage = Format::Format(fmtString.data(), i, ObjectTypeToString(object.objectType), object.objectHandle, (object.pObjectName ? object.pObjectName : ""));
                Logger::s_DefaultLogger->LogSimple(logLevel, objectInfoMessage);
            }
        }

        return VK_FALSE;
    }
}
