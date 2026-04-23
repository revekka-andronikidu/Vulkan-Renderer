#pragma once
#include "VulkanInstance.h"
#include "Device.h"
#include "Surface.h"
#include "../../Window.h"


class VulkanContext final
{
public:
	VulkanContext(Window& window)
		: m_Instance()
		, m_Surface(m_Instance.GetInstance(), window)
		, m_Device(m_Surface.GetSurface(), m_Instance)
	{
	}

	~VulkanContext() = default;

	VulkanInstance& GetInstance() { return m_Instance; }
	Device& GetDevice() { return m_Device; }
	VkSurfaceKHR GetSurface() { return m_Surface.GetSurface(); }


	VulkanContext(const VulkanContext&) = delete;
	VulkanContext& operator=(const VulkanContext&) = delete;
	VulkanContext(VulkanContext&&) = delete;
	VulkanContext& operator=(VulkanContext&&) = delete;

private:
	VulkanInstance m_Instance;
	Surface m_Surface;
	Device m_Device; 

	
};