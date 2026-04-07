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
    
    glfwGetFramebufferSize(windowPtr, &glwidth, &glheight);
    while (glwidth == 0 || glheight == 0) {
        glfwGetFramebufferSize(windowPtr, &glwidth, &glheight);
        glfwWaitEvents();
    }
    
}

static void frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto plat = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
	plat->frameBufferResized = true;
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    auto plt = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
	
    // Map GLFW keys to your Enum
    if (action == GLFW_REPEAT) return;
    Input::Key genericKey;
    switch (key) {
        case GLFW_KEY_W:      genericKey = Input::Key::Forward;  break;
        case GLFW_KEY_S:      genericKey = Input::Key::Backward; break;
        case GLFW_KEY_A:      genericKey = Input::Key::Left;     break;
        case GLFW_KEY_D:      genericKey = Input::Key::Right;    break;
        case GLFW_KEY_SPACE:  genericKey = Input::Key::Jump;     break;
        case GLFW_KEY_LEFT_SHIFT: genericKey = Input::Key::Sprint; break;
        default: return; // Ignore unmapped keys
    }

    // Binary Mechanic: Create the mask and update the 'current' bitfield
    uint64_t mask = (1ULL << static_cast<uint8_t>(genericKey));

    if (action == GLFW_PRESS) {
        plt->inputState.current |= mask;  // Bitwise OR: Set bit to 1
    } else if (action == GLFW_RELEASE) {
        plt->inputState.current &= ~mask; // Bitwise AND-NOT: Set bit to 0
    }
}

static void mouseKeyCallback(GLFWwindow* window, int button, int action, int mods){
     auto plt = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
	
    // Map GLFW keys to your Enum
    if (action == GLFW_REPEAT) return;
    Input::Key genericKey;
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:      genericKey = Input::Key::LeftClick;  break;
        case GLFW_MOUSE_BUTTON_RIGHT:      genericKey = Input::Key::RightClick; break;
        
        default: return; // Ignore unmapped keys
    }

    // Binary Mechanic: Create the mask and update the 'current' bitfield
    uint64_t mask = (1ULL << static_cast<uint8_t>(genericKey));

    if (action == GLFW_PRESS) {
        plt->inputState.current |= mask;  // Bitwise OR: Set bit to 1
    } else if (action == GLFW_RELEASE) {
        plt->inputState.current &= ~mask; // Bitwise AND-NOT: Set bit to 0
    }
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
    glfwSetKeyCallback(windowPtr,keyCallback);
    glfwSetMouseButtonCallback(windowPtr, mouseKeyCallback);
   glwidth = width;
   glheight = height;
}
float PlatformGLFW::getWindowAspect(){

    return (float)glwidth/(float)glheight;
}