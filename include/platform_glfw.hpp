
#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
class PlatformGLFW{
  public:
  GLFWwindow* windowPtr;
  bool windowResized = false;
  
  void initWindow(int width, int height);
  int windowOpen();
  void pollEvents();
  const char** getInstanceExtensions(uint32_t* count);
  void createWindowSurface(VkInstance instance,VkSurfaceKHR* surface);
  void shutdown();
  void stallMinimizedWindow();
};