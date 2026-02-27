#include <iostream>
#include "platform_glfw.hpp"




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
void PlatformGLFW::stallMinimizedWindow(){
    
    glfwGetFramebufferSize(windowPtr, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(windowPtr, &width, &height);
        glfwWaitEvents();
    }
    
}

static void frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto plat = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
	plat->frameBufferResized = true;
}

void PlatformGLFW::initWindow(int width,int height) {
    if (width == 0 || height == 0){
        throw std::runtime_error("width or height zero??!!");
    }

	if (!glfwInit()) {
		throw std::runtime_error("glfw init not working");
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	windowPtr = glfwCreateWindow(width, height, " ayo wassup", nullptr, nullptr);
	//glfwSetInputMode(wtx->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(windowPtr, this);
	glfwSetFramebufferSizeCallback(windowPtr, frameBufferResizeCallback);

}