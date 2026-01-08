#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/vulkan/vulkan.h>
#include <onyx/platform/window.h>

namespace Onyx::Graphics
{
	class Window;
}

namespace Onyx::Graphics::Vulkan
{
	class Instance;
	class Surface : public NonCopyable
	{
	public:
		Surface(const Instance& instance, const Platform::Window& window);
		~Surface();

	private:
		const Instance& m_Instance;
		VULKAN_HANDLE(VkSurfaceKHR, Surface, nullptr);
	};

}