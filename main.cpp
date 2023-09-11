#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "cubecraft/CubeCraft.h"

float lastX = cubecraft::WIDTH / 2.0f;
float lastY = cubecraft::HEIGHT / 2.0f;
bool firstMouse = true;

//键盘输入
void processInput(GLFWwindow* window)
{
    auto& camera = cubecraft::Context::Instance().camera;
    auto deltaTime = 0.0025f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = 0.0025f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(UPWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(DOWNWARD, deltaTime);
    }
}
//鼠标输入
static void cursor_position_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    auto& camera = cubecraft::Context::Instance().camera;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

int main() {
    float w = cubecraft::WIDTH;
    float h = cubecraft::HEIGHT;

    GLFWwindow* window;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(cubecraft::WIDTH, cubecraft::HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//隐藏光标
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//显示光标
    glfwSetCursorPosCallback(window, cursor_position_callback);//鼠标回调函数

    try {
        cubecraft::Init(window);
        auto renderer = cubecraft::Context::Instance().GetRenderer();

        cubecraft::Texture* texture = cubecraft::LoadTexture("resources/dirt.png");

        while (!glfwWindowShouldClose(window)) {
            renderer->StartRender();
            renderer->DrawTexture(*texture);
            renderer->EndRender();

            processInput(window);
            glfwPollEvents();
        }
        
        cubecraft::DestroyTexture(texture);

        cubecraft::Quit();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}