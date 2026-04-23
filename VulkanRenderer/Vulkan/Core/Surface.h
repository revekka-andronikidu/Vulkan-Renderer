#pragma once
#include "../../Window.h"
#include <vulkan/vulkan.h>	
#include <stdexcept>

class Surface final
{
public:
	Surface(VkInstance instance, Window& window)
		: m_Instance(instance)
		, m_WindowRef(window)
		, m_Surface(VK_NULL_HANDLE)
	{
		CreateSurface();
	}
	~Surface()
	{
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

	void CreateSurface()
	{
		if (glfwCreateWindowSurface(m_Instance, m_WindowRef.GetWindow(), nullptr, &m_Surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	VkSurfaceKHR GetSurface() const { return m_Surface; }

	private:
		VkInstance m_Instance;
		Window& m_WindowRef;
		VkSurfaceKHR m_Surface;
};