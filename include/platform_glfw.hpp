
#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "input_format.hpp"
class PlatformGLFW{
  public:
  GLFWwindow* windowPtr;
  bool frameBufferResized = false;
  int glwidth{};
  int glheight{};
  Input::State inputState{};
  void initWindow(int width, int height);
  int windowOpen();
  void pollEvents();
  const char** getInstanceExtensions(uint32_t* count);
  void createWindowSurface(VkInstance instance,VkSurfaceKHR* surface);
  void shutdown();
  void stallMinimizedWindow();
  float getWindowAspect();
  

  
};