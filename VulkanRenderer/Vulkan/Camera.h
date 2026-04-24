#pragma once
#include <chrono>
#include "Frame/UniformBuffer.h"
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera() = default;


    void UpdateUniformBuffer(uint32_t currentImage, UniformBuffer& uniformBuffer, VkExtent2D swapChainExtent)
    {
        UniformBufferObject ubo{};

        // No rotation — static scene
        ubo.model = glm::mat4(1.0f);

        // Pulled back to see the full Sponza courtyard
        ubo.view = glm::lookAt(
            glm::vec3(0.0f, 2.0f, 5.0f),   // camera position — above and back
            glm::vec3(0.0f, 1.0f, 0.0f),   // look at center of scene
            glm::vec3(0.0f, 1.0f, 0.0f));  // Y is up

        ubo.proj = glm::perspective(
            glm::radians(60.0f),                                        // wider fov for large scene
            swapChainExtent.width / (float)swapChainExtent.height,
            0.1f, 1000.0f);                                             // far plane extended for Sponza

        ubo.proj[1][1] *= -1;
        uniformBuffer.Update(currentImage, ubo);
    }
};