#pragma once

#include <onyx/log/logger.h>
#include <vulkan/vulkan.h>
#include <onyx/graphics/graphicstypes.h>

#if ONYX_IS_ANDROID
#include <onyx/graphics/vulkan/vulkanandroid.h>
#endif

namespace Onyx::Graphics::Vulkan
{
	inline PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV = nullptr;
	inline PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV = nullptr;
	inline PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV = nullptr;
	inline PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;
	inline PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = nullptr;
	inline PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = nullptr;

	inline String errorString(VkResult errorCode)
	{
		switch (errorCode)
		{
#define STR(r) case VK_ ##r: return #r
			STR(NOT_READY);
			STR(TIMEOUT);
			STR(EVENT_SET);
			STR(EVENT_RESET);
			STR(INCOMPLETE);
			STR(ERROR_OUT_OF_HOST_MEMORY);
			STR(ERROR_OUT_OF_DEVICE_MEMORY);
			STR(ERROR_INITIALIZATION_FAILED);
			STR(ERROR_DEVICE_LOST);
			STR(ERROR_MEMORY_MAP_FAILED);
			STR(ERROR_LAYER_NOT_PRESENT);
			STR(ERROR_EXTENSION_NOT_PRESENT);
			STR(ERROR_FEATURE_NOT_PRESENT);
			STR(ERROR_INCOMPATIBLE_DRIVER);
			STR(ERROR_TOO_MANY_OBJECTS);
			STR(ERROR_FORMAT_NOT_SUPPORTED);
			STR(ERROR_SURFACE_LOST_KHR);
			STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
			STR(SUBOPTIMAL_KHR);
			STR(ERROR_OUT_OF_DATE_KHR);
			STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
			STR(ERROR_VALIDATION_FAILED_EXT);
			STR(ERROR_INVALID_SHADER_NV);
#undef STR
		default:
			return "UNKNOWN_ERROR";
		}
	}

	inline void SetResourceName(VkDevice device, VkObjectType type, onyxU64 objHandle, const StringView& name)
    {
		if (vkSetDebugUtilsObjectNameEXT == nullptr)
			return;
		
		VkDebugUtilsObjectNameInfoEXT name_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
		name_info.objectType = type;
		name_info.objectHandle = objHandle;
		name_info.pObjectName = name.data();
		vkSetDebugUtilsObjectNameEXT(device, &name_info);
	}

	inline constexpr ShaderStage ToShaderStage(VkShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT:
			return ShaderStage::Vertex;
		case VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT:
			return ShaderStage::Fragment;
		case VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT:
			return ShaderStage::Compute;
		case VkShaderStageFlagBits::VK_SHADER_STAGE_ALL:
			return ShaderStage::All;
		default:
			ONYX_LOG_ERROR("Unhandled shader stage: %s", Enums::ToString(stage).data());
			return ShaderStage::Invalid;
		}
	}

	inline constexpr VkShaderStageFlagBits ToVulkanStage(ShaderStage stage)
	{
		switch (stage)
		{
		case ShaderStage::Vertex:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderStage::Fragment:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderStage::Compute:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
		case ShaderStage::All:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		default:
			ONYX_LOG_ERROR("Unhandled shader stage: %s", Enums::ToString(stage).data());
			return VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		}
	}

	inline VkPipelineStageFlags GetPipelineFlags(VkAccessFlags access, Context context)
    {
		VkPipelineStageFlags flags = 0;

		switch (context)
		{
		case Context::Graphics:
		{
			if ((access & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

			if ((access & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0) {
				flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
				flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

				// TODO(marco): check RT extension is present/enabled
				flags |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
			}

			if ((access & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0)
				flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			if ((access & (VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)) != 0)
				flags |= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

			if ((access & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			if ((access & VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR) != 0)
				flags = VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

			if ((access & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

			break;
		}
		case Context::Compute:
		{
			if ((access & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0 ||
				(access & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0 ||
				(access & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0 ||
				(access & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
				return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			if ((access & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

			break;
		}
		case Context::CopyTransfer: return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		default: break;
		}

		// Compatible with both compute and graphics queues
		if ((access & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) != 0)
			flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

		if ((access & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;

		if ((access & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_HOST_BIT;

		if (flags == 0)
			flags = VK_PIPELINE_STAGE_HOST_BIT;

		return flags;
	}

	inline VkPipelineStageFlags2KHR GetPipelineFlags(VkAccessFlags2KHR access_flags, Context context)
    {
	    VkPipelineStageFlags2KHR flags = 0;

		switch (context)
	    {
		case Context::Graphics:
		{
			if ((access_flags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT_KHR;

			if ((access_flags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0) {
				flags |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR;
				flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR;
				flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
#ifdef ENABLE_RAYTRACING
				if (pRenderer->mVulkan.mRaytracingExtension) {
					flags |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV;
				}
#endif
			}

			if ((access_flags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0)
				flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR;

			if ((access_flags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR;

			if ((access_flags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR;

			break;
		}
		case Context::Compute:
		{
			if ((access_flags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0 ||
				(access_flags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0 ||
				(access_flags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0 ||
				(access_flags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
				return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;

			if ((access_flags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
				flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;

			break;
		}
		case Context::CopyTransfer: return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
		default: break;
		}

		// Compatible with both compute and graphics queues
		if ((access_flags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) != 0)
			flags |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT_KHR;

		if ((access_flags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR;

		if ((access_flags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_2_HOST_BIT_KHR;

		if (flags == 0)
			flags = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT_KHR;

		return flags;
	}
}

// Custom define for better code readability
#define VK_FLAGS_NONE 0
// Default fence timeout in nanoseconds
#define DEFAULT_FENCE_TIMEOUT 100000000000

// Macro to check and display Vulkan return results
#if defined(__ANDROID__)
#define VK_CHECK_RESULT(f)															\
{																					\
	VkResult res = (f);																\
	if (res != VK_SUCCESS)															\
	{																				\
		ONYX_LOG_FATAL("VkResult is %s ", vks::tools::errorString(res)));	\
		assert(res == VK_SUCCESS);													\
	}																				\
}
#else
#define VK_CHECK_RESULT(f)						\
{												\
	VkResult res = (f);							\
	if (res != VK_SUCCESS)						\
	{											\
		ONYX_LOG_FATAL("VkResult is %s", Onyx::Graphics::Vulkan::errorString(res));	\
		ONYX_ASSERT(res == VK_SUCCESS);			\
	}											\
}

#define VK_CHECK_RESULT_RETURN_ON_FAIL(f)					\
{												\
	VkResult res = (f);							\
	if (res != VK_SUCCESS)						\
	{											\
		ONYX_LOG_FATAL("VkResult is %s", Onyx::Graphics::Vulkan::errorString(res));	\
		ONYX_ASSERT(res == VK_SUCCESS);			\
	    return false;							\
	}											\
}
#endif

#define VULKAN_HANDLE(VulkanHandleType, name, defaultValue)				\
public:																	\
    VulkanHandleType* GetHandlePtr() { return &m_##name; }				\
    const VulkanHandleType* GetHandlePtr() const { return &m_##name; }	\
    VulkanHandleType GetHandle() { return m_##name; }					\
	const VulkanHandleType GetHandle() const { return m_##name; }		\
private:																\
	VulkanHandleType m_##name = defaultValue;