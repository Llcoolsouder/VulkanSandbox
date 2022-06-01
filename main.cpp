#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <vector>

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
        mpWindow = glfwCreateWindow(mWidth, mHeight, mpAppName, nullptr, nullptr);
    }

    void initVulkan()
    {
        createInstance();
    }

    void createInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = mpAppName;
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        uint32_t glfwExtensionCount = 0;
        char const **ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        uint32_t supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

#ifndef NDEBUG
        std::cout << "Available extensions:\n";
        for (const auto &extension : supportedExtensions)
        {
            std::cout << '\t' << extension.extensionName << '\n';
        }
#endif

        for (unsigned int i = 0; i < glfwExtensionCount; ++i)
        {
            auto const pGlfwExtensionName = ppGlfwExtensions[i];
            bool extensionNotSupported = supportedExtensions.end() == std::find_if(supportedExtensions.begin(), supportedExtensions.end(), [&](auto const extension)
                                                                                   { return std::strcmp(extension.extensionName, pGlfwExtensionName) == 0; });
            if (extensionNotSupported)
            {
                throw std::runtime_error("Vulkan extension not supported: " + std::string(pGlfwExtensionName));
            }
        }

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = glfwExtensionCount;
        instanceInfo.ppEnabledExtensionNames = ppGlfwExtensions;
        instanceInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&instanceInfo, nullptr, &mInstance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance");
        }
    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(mpWindow))
        {
            glfwPollEvents();
        }
    }

    void cleanup()
    {
        vkDestroyInstance(mInstance, nullptr);
        glfwDestroyWindow(mpWindow);
        glfwTerminate();
    }

private:
    static constexpr uint32_t mWidth = 800;
    static constexpr uint32_t mHeight = 600;
    static constexpr char mpAppName[] = "Vulkan Tutorial";
    GLFWwindow *mpWindow;

    VkInstance mInstance;
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