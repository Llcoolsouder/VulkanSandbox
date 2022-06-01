#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <vector>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
    void *pUserData)
{
    std::ostream &out = messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ? std::cerr : std::cout;
    auto const printMessage = [&]()
    {
        out << "Validation Layer: " << pCallbackData->pMessage << std::endl;
    };

#ifdef NDEBUG
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        printMessage();
    }
#else
    printMessage();
#endif

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo, VkAllocationCallbacks const *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkAllocationCallbacks const *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

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
        setupDebugMessenger();
    }

    VkDebugUtilsMessengerCreateInfoEXT getDefaultDebugUtilsMessengerCreateInfoEXT()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
        return createInfo;
    }

    void setupDebugMessenger()
    {
        if (!mEnableValidationLayers)
            return;

        auto const createInfo = getDefaultDebugUtilsMessengerCreateInfoEXT();

        if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger");
        }
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

        if (mEnableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested but not available");
        }

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        auto const extensions = getRequireExtensions();
        instanceInfo.enabledExtensionCount = extensions.size();
        instanceInfo.ppEnabledExtensionNames = extensions.data();
        auto const debugMessengerCreateInfo = getDefaultDebugUtilsMessengerCreateInfoEXT();
        if (mEnableValidationLayers)
        {
            instanceInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
            instanceInfo.ppEnabledLayerNames = mValidationLayers.data();
            instanceInfo.pNext = &debugMessengerCreateInfo;
        }
        else
        {
            instanceInfo.enabledLayerCount = 0;
            instanceInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&instanceInfo, nullptr, &mInstance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance");
        }
    }

    bool checkValidationLayerSupport()
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (auto const &layerName : mValidationLayers)
        {
            bool const layerNotAvailable = availableLayers.end() == std::find_if(availableLayers.begin(), availableLayers.end(), [&](auto const layer)
                                                                                 { return std::strcmp(layer.layerName, layerName) == 0; });
            if (layerNotAvailable)
            {
                return false;
            }
        }
        return true;
    }

    std::vector<char const *> getRequireExtensions()
    {
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

        std::vector<char const *> extensions(ppGlfwExtensions, ppGlfwExtensions + glfwExtensionCount);
        if (mEnableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        for (auto const pExtensionName : extensions)
        {
            bool extensionNotSupported = supportedExtensions.end() == std::find_if(supportedExtensions.begin(), supportedExtensions.end(), [&](auto const extension)
                                                                                   { return std::strcmp(extension.extensionName, pExtensionName) == 0; });
            if (extensionNotSupported)
            {
                throw std::runtime_error("Vulkan extension not supported: " + std::string(pExtensionName));
            }
        }

        return extensions;
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
        if (mEnableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
        }
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
    VkDebugUtilsMessengerEXT mDebugMessenger;

    const std::vector<char const *> mValidationLayers = {"VK_LAYER_KHRONOS_validation"};
#ifdef NDEBUG
    const bool mEnableValidationLayers = false;
#else
    const bool mEnableValidationLayers = true;
#endif
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