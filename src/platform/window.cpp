#include <iostream>
#include "platform_glfw.hpp"




int PlatformGLFW::windowOpen(){
    return !glfwWindowShouldClose(windowPtr);
} 
void PlatformGLFW::updateState(){
    //necessary for testing holding,releasing and presssing keys
    inputState.previous = inputState.current;
    
    glfwPollEvents();
    
    //just deltaTime
    static float lastTime = static_cast<float>(glfwGetTime());
    float currTime = static_cast<float>(glfwGetTime());
    deltaTime = currTime-lastTime;
    
    lastTime = currTime;
    static bool incomingRequest = inputState.requestCursorVisible;

    if(incomingRequest != inputState.requestCursorVisible){
        glfwSetInputMode(windowPtr, GLFW_CURSOR, inputState.requestCursorVisible ? (GLFW_CURSOR_NORMAL) : (GLFW_CURSOR_DISABLED)); 

        incomingRequest = inputState.requestCursorVisible;
    }
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
    
    
    aspectRatio =  (float)glwidth/(float)glheight;


    
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
        case GLFW_KEY_ESCAPE: genericKey = Input::Key::Escape; break;
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

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos){
    auto plt = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
    if (plt->inputState.requestCursorVisible) {
        // 1. Zero out deltas so the game doesn't process movement
        plt->inputState.mouseDX = 0;
        plt->inputState.mouseDY = 0;
        
        // 2. Keep lastX/Y updated so when we hide the cursor again, 
        // there isn't a massive "snap" from the old position.
        plt->inputState.lastX = xpos;
        plt->inputState.lastY = ypos;
        return;
    }

    plt->inputState.mouseDX  = xpos - plt->inputState.lastX;
    plt->inputState.mouseDY  = plt->inputState.lastY - ypos;

    
    plt->inputState.lastX = xpos;
    plt->inputState.lastY = ypos;

    

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

    glfwSetCursorPosCallback(windowPtr,cursorPosCallback);
    glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    // if (glfwRawMouseMotionSupported())
    //     glfwSetInputMode(windowPtr, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

   glwidth = width;
   glheight = height;
   aspectRatio =  (float)glwidth/(float)glheight;
}

