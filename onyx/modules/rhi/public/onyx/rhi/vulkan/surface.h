#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>
#include <onyx/platform/window.h>

namespace onyx::rhi
{
	class Window;
}

namespace onyx::rhi::vulkan
{
	class Instance;
	class Surface : public NonCopyable
	{
	public:
		Surface(const Instance& instance, const platform::Window& window);
		~Surface();

	private:
		const Instance& m_Instance;
		VULKAN_HANDLE(VkSurfaceKHR, Surface, nullptr);
	};

}