#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "cubecraft/Context.h"


class CubeCraft {
public:
    void run() {
        Init();
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    cubecraft::Context context;

    void Init() {
        context.Init();
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

int main() {
    CubeCraft app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}