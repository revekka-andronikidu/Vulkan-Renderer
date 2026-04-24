#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "Frame/UniformBuffer.h"


class Camera
{
public:
    Camera()
        : m_Position(0.0f, 0.0f, 0.0f)
        , m_Front(1.0f, 0.0f, 0.0f)
        , m_Up(0.0f, 1.0f, 0.0f)
        , m_Yaw(-0.0f)
        , m_Pitch(0.0f)
        , m_MoveSpeed(200.0f)
        , m_LookSensitivity(0.1f)
        , m_Fov(45.0f)
        , m_LastMouseX(0.0f)
        , m_LastMouseY(0.0f)
        , m_FirstMouse(true)
        , m_MouseCaptured(false)
    {
    }


    void ProcessInput(GLFWwindow* window, float deltaTime)
    {
        // Toggle mouse capture with Escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            m_MouseCaptured = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            m_MouseCaptured = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        const float velocity = m_MoveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_Position += m_Front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_Position -= m_Front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_Position -= glm::normalize(glm::cross(m_Front, m_Up)) * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_Position += glm::normalize(glm::cross(m_Front, m_Up)) * velocity;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            m_Position -= m_Up * velocity;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            m_Position += m_Up * velocity;
    }

    void ProcessMouseMovement(float xpos, float ypos)
    {
        if (!m_MouseCaptured)
            return;

        if (m_FirstMouse)
        {
            m_LastMouseX = xpos;
            m_LastMouseY = ypos;
            m_FirstMouse = false;
        }

        float xOffset = (xpos - m_LastMouseX) * m_LookSensitivity;
        float yOffset = (m_LastMouseY - ypos) * m_LookSensitivity;
        m_LastMouseX = xpos;
        m_LastMouseY = ypos;

        m_Yaw += xOffset;
        m_Pitch = glm::clamp(m_Pitch + yOffset, -89.0f, 89.0f);

        UpdateFrontVector();
    }

    void ProcessMouseScroll(float yOffset)
    {
        m_Fov = glm::clamp(m_Fov - yOffset, 1.0f, 90.0f);
    }

    void UpdateUniformBuffer(uint32_t currentImage, UniformBuffer& uniformBuffer, VkExtent2D swapChainExtent) const
    {
        UniformBufferObject ubo{};
        ubo.model = glm::mat4(1.0f);  
        ubo.view = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        ubo.proj = glm::perspective(glm::radians(m_Fov), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 5000.0f); 
        ubo.proj[1][1] *= -1;

        uniformBuffer.Update(currentImage, ubo);
    }

    // Setters for tuning
    void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    void SetSensitivity(float sens) { m_LookSensitivity = sens; }
    void SetPosition(glm::vec3 position) { m_Position = position; }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;

    float m_Yaw;
    float m_Pitch;
    float m_MoveSpeed;
    float m_LookSensitivity;
    float m_Fov;

    float m_LastMouseX;
    float m_LastMouseY;
    bool  m_FirstMouse;
    bool  m_MouseCaptured;

    void UpdateFrontVector()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);
    }
};