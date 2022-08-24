#include "HelloTriangleApplication.h"

//==============================================================================
// Constants
//==============================================================================
static constexpr uint32_t WIDTH = 800;
static constexpr uint32_t HEIGHT = 600;
static constexpr char APP_NAME[] = "Vulkan Tutorial";
#ifdef NDEBUG
static constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
static constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif
static std::vector<char const *> const VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"};

//==============================================================================
// Helper Declarations
//==============================================================================

namespace {
GLFWwindow *InitWindow();

VkInstance CreateInstance();

VkSurfaceKHR CreateSurface(VkInstance const &instance, GLFWwindow *pWindow);

std::vector<char const *> GetRequiredExtensions();

VkPhysicalDevice PickPhysicalDevice(VkInstance const &instance);

bool IsDeviceSuitable(VkPhysicalDevice const &device);

VkDevice CreateLogicalDevice(VkPhysicalDevice const &physicalDevice,
                             QueueFamilyIndices const &queueFamilyIndices);

VkQueue GetGraphicsQueueHandle(VkDevice const &device,
                               QueueFamilyIndices const &queueFamilyIndices);

VkQueue GetPresentQueueHandle(VkDevice const &device,
                              QueueFamilyIndices const &queueFamilyIndices);

VkDebugUtilsMessengerEXT SetupDebugMessenger(VkInstance const &instance);

bool CheckValidationLayerSupport();

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback();

VkDebugUtilsMessengerCreateInfoEXT GetDefaultDebugUtilsMessengerCreateInfoEXT();

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
    VkAllocationCallbacks const *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   VkAllocationCallbacks const *pAllocator);

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice const &device,
                                     VkSurfaceKHR const &surface);
} // namespace

//==============================================================================
// Definitions
//==============================================================================
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HelloTriangleApplication::HelloTriangleApplication()
    : mpWindow(InitWindow()),
      mInstance(CreateInstance()),
      mDebugMessenger(SetupDebugMessenger(mInstance)),
      mSurface(CreateSurface(mInstance, mpWindow)),
      mPhysicalDevice(PickPhysicalDevice(mInstance)),
      mQueueFamilyIndices(FindQueueFamilies(mPhysicalDevice, mSurface)),
      mDevice(CreateLogicalDevice(mPhysicalDevice, mQueueFamilyIndices)),
      mGraphicsQueue(GetGraphicsQueueHandle(mDevice, mQueueFamilyIndices)) {}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HelloTriangleApplication::~HelloTriangleApplication() {
  vkDestroyDevice(mDevice, nullptr);
  if (ENABLE_VALIDATION_LAYERS) {
    DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
  }
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  vkDestroyInstance(mInstance, nullptr);
  glfwDestroyWindow(mpWindow);
  glfwTerminate();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void HelloTriangleApplication::Run() {
  while (!glfwWindowShouldClose(mpWindow)) {
    glfwPollEvents();
  }
}

namespace {

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
GLFWwindow *InitWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  return glfwCreateWindow(WIDTH, HEIGHT, APP_NAME, nullptr, nullptr);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkInstance CreateInstance() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = APP_NAME;
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport()) {
    throw std::runtime_error("Validation layers requested but not available");
  }

  VkInstanceCreateInfo instanceInfo{};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &appInfo;
  auto const extensions = GetRequiredExtensions();
  instanceInfo.enabledExtensionCount = extensions.size();
  instanceInfo.ppEnabledExtensionNames = extensions.data();
  auto const debugMessengerCreateInfo =
      GetDefaultDebugUtilsMessengerCreateInfoEXT();
  if (ENABLE_VALIDATION_LAYERS) {
    instanceInfo.enabledLayerCount =
        static_cast<uint32_t>(VALIDATION_LAYERS.size());
    instanceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    instanceInfo.pNext = &debugMessengerCreateInfo;
  } else {
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.pNext = nullptr;
  }

  VkInstance instance;
  if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan instance");
  }
  return instance;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkSurfaceKHR CreateSurface(VkInstance const &instance, GLFWwindow *pWindow) {
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(instance, pWindow, nullptr, &surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("Unable to create window surface");
  }
  return surface;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<char const *> GetRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  char const **ppGlfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  uint32_t supportedExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(
      nullptr, &supportedExtensionCount, nullptr);
  std::vector<VkExtensionProperties> supportedExtensions(
      supportedExtensionCount);
  vkEnumerateInstanceExtensionProperties(
      nullptr, &supportedExtensionCount, supportedExtensions.data());

#ifndef NDEBUG
  std::cout << "Available extensions:\n";
  for (const auto &extension : supportedExtensions) {
    std::cout << '\t' << extension.extensionName << '\n';
  }
#endif

  std::vector<char const *> extensions(ppGlfwExtensions,
                                       ppGlfwExtensions + glfwExtensionCount);
  if (ENABLE_VALIDATION_LAYERS) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  for (auto const pExtensionName : extensions) {
    bool extensionNotSupported =
        supportedExtensions.end() ==
        std::find_if(supportedExtensions.begin(),
                     supportedExtensions.end(),
                     [&](auto const extension) {
                       return std::strcmp(extension.extensionName,
                                          pExtensionName) == 0;
                     });
    if (extensionNotSupported) {
      throw std::runtime_error("Vulkan extension not supported: " +
                               std::string(pExtensionName));
    }
  }

  return extensions;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkPhysicalDevice PickPhysicalDevice(VkInstance const &instance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("Failed to find GPU with Vulkan support!");
  }
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  // Get first suitable device
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  for (auto const &device : devices) {
    if (IsDeviceSuitable(device)) {
      physicalDevice = device;
      break;
    }
  }
  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a suitable GPU");
  }
  return physicalDevice;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool IsDeviceSuitable(VkPhysicalDevice const &device) {
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
         deviceFeatures.geometryShader;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkDevice CreateLogicalDevice(VkPhysicalDevice const &physicalDevice,
                             QueueFamilyIndices const &queueFamilyIndices) {

  auto const uniqueQueueFamilyIndices = queueFamilyIndices.ToSet();
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());
  float const queuePriority = 1.0f;
  for (auto const uniqueQueueFamilyIndex : uniqueQueueFamilyIndices) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = uniqueQueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures physicsalDeviceFeatures{};

  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.enabledExtensionCount = 0;

  if (ENABLE_VALIDATION_LAYERS) {
    deviceCreateInfo.enabledLayerCount = VALIDATION_LAYERS.size();
    deviceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
  } else {
    deviceCreateInfo.enabledLayerCount = 0;
  }

  VkDevice device;
  if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) !=
      VK_SUCCESS) {
    throw std::runtime_error("Unable to create Vulkan logical device");
  }
  return device;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkQueue GetGraphicsQueueHandle(VkDevice const &device,
                               QueueFamilyIndices const &queueFamilyIndices) {
  VkQueue queueHandle;
  vkGetDeviceQueue(
      device, queueFamilyIndices.graphicsFamily.value(), 0, &queueHandle);
  return queueHandle;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkQueue GetPresentQueueHandle(VkDevice const &device,
                              QueueFamilyIndices const &queueFamilyIndices) {
  VkQueue queueHandle;
  vkGetDeviceQueue(
      device, queueFamilyIndices.presentFamily.value(), 0, &queueHandle);
  return queueHandle;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkDebugUtilsMessengerEXT SetupDebugMessenger(VkInstance const &instance) {
  if (!ENABLE_VALIDATION_LAYERS) {
    return VK_NULL_HANDLE;
  }

  auto const createInfo = GetDefaultDebugUtilsMessengerCreateInfoEXT();

  VkDebugUtilsMessengerEXT debugMessenger;
  if (CreateDebugUtilsMessengerEXT(
          instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger");
  }
  return debugMessenger;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool CheckValidationLayerSupport() {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (auto const &layerName : VALIDATION_LAYERS) {
    bool const layerNotAvailable =
        availableLayers.end() ==
        std::find_if(availableLayers.begin(),
                     availableLayers.end(),
                     [&](auto const layer) {
                       return std::strcmp(layer.layerName, layerName) == 0;
                     });
    if (layerNotAvailable) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
              void *pUserData) {
  std::ostream &out =
      messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
          ? std::cerr
          : std::cout;
  auto const printMessage = [&]() {
    out << "Validation Layer: " << pCallbackData->pMessage << std::endl;
  };

#ifdef NDEBUG
  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    printMessage();
  }
#else
  printMessage();
#endif

  return VK_FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkDebugUtilsMessengerCreateInfoEXT
GetDefaultDebugUtilsMessengerCreateInfoEXT() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;
  createInfo.pUserData = nullptr;
  return createInfo;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
    VkAllocationCallbacks const *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   VkAllocationCallbacks const *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice const &device,
                                     VkSurfaceKHR const &surface) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &queueFamilyCount, queueFamilyProperties.data());

  for (uint32_t i = 0; i < queueFamilyCount; ++i) {
    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.IsComplete()) {
      break;
    }
  }

  if (!indices.IsComplete()) {
    throw std::runtime_error(
        "Selected device does not support all required queue families");
  }

  return indices;
}

} // namespace

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool QueueFamilyIndices::IsComplete() const {
  return graphicsFamily.has_value() && presentFamily.has_value();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::set<uint32_t> QueueFamilyIndices::ToSet() const {
  return {graphicsFamily.value(), presentFamily.value()};
}
