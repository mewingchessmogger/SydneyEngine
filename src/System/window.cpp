#include <iostream>
#include "platform_glfw.hpp"
void PlatformGLFW::initWindow(int width,int height){
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return;
    }
    // IMPORTANT: Explicitly disable OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Recommended for early Vulkan dev

    windowPtr = glfwCreateWindow(width, height, "SydneyEngine", nullptr, nullptr);

    if (!windowPtr) {
        glfwTerminate();
        return;
    }
 // Check if Vulkan is actually available on the system
    if (!glfwVulkanSupported()) {
        std::cout << "Vulkan Not Supported!" << std::endl;
        return;
    }

}
int PlatformGLFW::windowOpen(){
    return !glfwWindowShouldClose(windowPtr);
} 
void PlatformGLFW::pollEvents(){
    glfwPollEvents();
}

const char** PlatformGLFW::getInstanceExtensions(uint32_t* count)
{
    return glfwGetRequiredInstanceExtensions(count);
}

void PlatformGLFW::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
{
    glfwCreateWindowSurface(instance, windowPtr, nullptr,surface);
}

void PlatformGLFW::shutdown()
{
    glfwTerminate();
}
