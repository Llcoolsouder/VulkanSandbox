#ifndef _HELLO_TRIANGLE_APPLICATION_H_
#define _HELLO_TRIANGLE_APPLICATION_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

class HelloTriangleApplication {
public:
  HelloTriangleApplication();

  ~HelloTriangleApplication();

  void Run();

private:
  GLFWwindow *mpWindow;
  VkInstance mInstance;
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT mDebugMessenger;
};

#endif //_HELLO_TRIANGLE_APPLICATION_H_