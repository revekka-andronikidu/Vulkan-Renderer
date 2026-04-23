#pragma once
#include "Vulkan/VulkanRenderer.h"

class VulkanApplication 
{
    public:
    void run() 
    {
		renderer.Run();
	}

private:

	VulkanRenderer renderer;
};

int main() 
{
    VulkanApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}