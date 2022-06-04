#ifndef _HELLO_TRIANGLE_APPLICATION_H_
#define _HELLO_TRIANGLE_APPLICATION_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  bool IsComplete() const;
};

class HelloTriangleApplication {
public:
  HelloTriangleApplication();

  ~HelloTriangleApplication();

  void Run();

private:
  GLFWwindow *mpWindow;
  VkInstance mInstance;
  VkPhysicalDevice mPhysicalDevice;
  QueueFamilyIndices mQueueFamilyIndices;
  VkDevice mDevice;
  VkQueue mGraphicsQueue;
  VkDebugUtilsMessengerEXT mDebugMessenger;
};

#endif //_HELLO_TRIANGLE_APPLICATION_H_