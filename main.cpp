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
    float w = cubecraft::WIDTH;
    float h = cubecraft::HEIGHT;

    GLFWwindow* window;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(cubecraft::WIDTH, cubecraft::HEIGHT, "Vulkan", nullptr, nullptr);

    try {
        cubecraft::Init(window);
        auto renderer = cubecraft::Context::GetInstance().GetRenderer();

        while (!glfwWindowShouldClose(window)) {
            renderer->StartRender();
            //renderer.SetDrawColor(cubecraft::Color{1, 0, 0});
            //renderer.DrawTexture(cubecraft::Rect{cubecraft::Vec{x, y}, cubecraft::Size{200, 300}}, * texture1);
            //renderer.SetDrawColor(cubecraft::Color{0, 1, 0});
            //renderer.DrawTexture(cubecraft::Rect{cubecraft::Vec{500, 100}, cubecraft::Size{200, 300}}, * texture2);
            //renderer.SetDrawColor(cubecraft::Color{0, 0, 1});
            //renderer.DrawLine(cubecraft::Vec{0, 0}, cubecraft::Vec{w, h});
            renderer->render();
            renderer->EndRender();

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