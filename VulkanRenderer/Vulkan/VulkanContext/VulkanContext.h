#pragma once
#include "VulkanInstance.h"
#include "../../Window.h"

class VulkanContext final
{
public:
	VulkanContext(Window& window)
		: window(window), instance(), surface(VK_NULL_HANDLE)	
	{
		CreateSurface();
	}

	~VulkanContext()
	{
		vkDestroySurfaceKHR(instance.GetInstance(), surface, nullptr);
	}

	VulkanInstance& GetInstance() { return instance; }
	VkSurfaceKHR& GetSurface() { return surface; }


	VulkanContext(const VulkanContext&) = delete;
	VulkanContext& operator=(const VulkanContext&) = delete;
	VulkanContext(VulkanContext&&) = delete;
	VulkanContext& operator=(VulkanContext&&) = delete;

private:
	VulkanInstance instance;
	VkSurfaceKHR surface;

	Window& window;

	void CreateSurface()
	{
		if (glfwCreateWindowSurface(instance.GetInstance(), window.GetWindow(), nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

};