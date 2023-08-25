#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "cubecraft/CubeCraft.h"

/*
class CubeCraft {
public:
    void run() {
        Init();
        Quit();
    }

private:
    cubecraft::Context context;

    void Init() {
        context.Init();
    }
    void Quit() {
        context.Quit();
    }

    void initWindow() {
        context.InitWindow();
    }

    void initVulkan() {
        context.InitVulkan();
    }

    void mainLoop() {
        context.MainLoop();
    }

    void cleanup() {
        context.QuitVulkan();
        context.QuitWindow();
    }
};
*/

int main() {
    GLFWwindow* window;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(cubecraft::WIDTH, cubecraft::HEIGHT, "Vulkan", nullptr, nullptr);

    try {
        cubecraft::Init(window);
        auto render = cubecraft::getRenderer();

        while (!glfwWindowShouldClose(window)) {
            render.Render();
            glfwPollEvents();
        }
        
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