#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
	class Device;

	class Semaphore : public NonCopyable
	{
	public:
		explicit Semaphore(const Device& device);
		explicit Semaphore(const Device& device, bool isTimelineSemaphore);
		Semaphore(Semaphore&& other) noexcept;
		~Semaphore();

	private:
		const Device& m_Device;
		VULKAN_HANDLE(VkSemaphore, Semaphore, nullptr)
	};
}
