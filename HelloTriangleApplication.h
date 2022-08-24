#ifndef _HELLO_TRIANGLE_APPLICATION_H_
#define _HELLO_TRIANGLE_APPLICATION_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() const;

  std::set<uint32_t> ToSet() const;
};

class HelloTriangleApplication {
public:
  HelloTriangleApplication();

  ~HelloTriangleApplication();

  void Run();

private:
  GLFWwindow *mpWindow;
  VkInstance mInstance;
  VkDebugUtilsMessengerEXT mDebugMessenger;
  VkSurfaceKHR mSurface;
  VkPhysicalDevice mPhysicalDevice;
  QueueFamilyIndices mQueueFamilyIndices;
  VkDevice mDevice;
  VkQueue mGraphicsQueue;
  VkQueue mPresentQueue;
};

#endif //_HELLO_TRIANGLE_APPLICATION_H_