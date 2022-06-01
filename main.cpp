#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <stdexcept>

class HelloTriangleApplication
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        pWindow = glfwCreateWindow(800, 600, "VulkanTutorial", nullptr, nullptr);
    }

    void initVulkan()
    {
    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(pWindow))
        {
            glfwPollEvents();
        }
    }

    void cleanup()
    {
        glfwDestroyWindow(pWindow);
        glfwTerminate();
    }

private:
    static constexpr uint32_t width = 800;
    static constexpr uint32_t height = 600;
    GLFWwindow *pWindow;
};

int main()
{
    HelloTriangleApplication app;

    try
    {
        app.run();
    }
    catch (std::exception const &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}