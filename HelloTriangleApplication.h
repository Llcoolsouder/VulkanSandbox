#ifndef _HELLO_TRIANGLE_APPLICATION_H_
#define _HELLO_TRIANGLE_APPLICATION_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <vector>


class HelloTriangleApplication
{
public:
    void run();

private:
    void initWindow();

    void initVulkan();

    VkDebugUtilsMessengerCreateInfoEXT getDefaultDebugUtilsMessengerCreateInfoEXT();

    void setupDebugMessenger();

    void createInstance();

    bool checkValidationLayerSupport();

    std::vector<char const *> getRequireExtensions();

    void mainLoop();

    void cleanup();

private:
    GLFWwindow *mpWindow;
    VkInstance mInstance;
    VkDebugUtilsMessengerEXT mDebugMessenger;
};

#endif  //_HELLO_TRIANGLE_APPLICATION_H_