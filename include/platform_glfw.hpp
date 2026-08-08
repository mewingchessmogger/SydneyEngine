
#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "input_format.hpp"

class PlatformGLFW{
  public:
  float deltaTime{};
  GLFWwindow* windowPtr;
  bool frameBufferResized = false;
  bool closeWindowRequested = false;
  int glwidth{};
  int glheight{};
  float aspectRatio;
  Input::State inputState{};
  void initWindow(int width, int height);
  int windowOpen();
  void updateState();
  static const char **getInstanceExtensions(uint32_t *count);
  static void createWindowSurface(void* windowPtr, VkInstance instance,VkSurfaceKHR* surface);
  void shutdown();
  static void stallMinimizedWindow(void* winPtr, int& glwidth, int& glheight, float& aspectRatio);
  float getWindowAspect();
  float getDeltaTime();

};