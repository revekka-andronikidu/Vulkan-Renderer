#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//const uint32_t WIDTH = 800;
//const uint32_t HEIGHT = 600;

class Window
{
public:
	Window(const std::string& title = "GP2 - Revekka Andronikidu", uint32_t width = 800, uint32_t height = 600)
		: m_title(title), m_width(width), m_height(height)
	{
		InitWindow();
	}
	~Window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
	bool ShouldClose() const
	{
		return glfwWindowShouldClose(m_window);
	}
	void PollEvents() const;
	VkSurfaceKHR CreateSurface(VkInstance instance) const;

	GLFWwindow* GetWindow() const { return m_window; }

	void InitWindow() 
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
	}


private:
	GLFWwindow* m_window;
	uint32_t m_width;
	uint32_t m_height;
	std::string m_title;
	bool framebufferResized = false;


	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}



};