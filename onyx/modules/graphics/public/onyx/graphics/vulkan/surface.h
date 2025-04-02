#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

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
		Surface(const Instance& instance, const Window& window);
		~Surface();

	private:
		const Instance& m_Instance;
		VULKAN_HANDLE(VkSurfaceKHR, Surface, nullptr);
	};

}