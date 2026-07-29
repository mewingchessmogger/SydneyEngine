#include <stdexcept>
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

void PlatformGLFW::createWindowSurface(void* windowPtr, VkInstance instance, VkSurfaceKHR *surface)
{
    glfwCreateWindowSurface(instance, static_cast<GLFWwindow*>(windowPtr), nullptr,surface);
}

void PlatformGLFW::shutdown()
{
    glfwTerminate();
}
void PlatformGLFW::stallMinimizedWindow(void* winPtr, int& glwidth, int& glheight, float& aspectRatio){
    GLFWwindow* windowPtr = static_cast<GLFWwindow*>(winPtr);
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
        // Letters
        case GLFW_KEY_A: genericKey = Input::Key::A; break;
        case GLFW_KEY_B: genericKey = Input::Key::B; break;
        case GLFW_KEY_C: genericKey = Input::Key::C; break;
        case GLFW_KEY_D: genericKey = Input::Key::D; break;
        case GLFW_KEY_E: genericKey = Input::Key::E; break;
        case GLFW_KEY_F: genericKey = Input::Key::F; break;
        case GLFW_KEY_G: genericKey = Input::Key::G; break;
        case GLFW_KEY_H: genericKey = Input::Key::H; break;
        case GLFW_KEY_I: genericKey = Input::Key::I; break;
        case GLFW_KEY_J: genericKey = Input::Key::J; break;
        case GLFW_KEY_K: genericKey = Input::Key::K; break;
        case GLFW_KEY_L: genericKey = Input::Key::L; break;
        case GLFW_KEY_M: genericKey = Input::Key::M; break;
        case GLFW_KEY_N: genericKey = Input::Key::N; break;
        case GLFW_KEY_O: genericKey = Input::Key::O; break;
        case GLFW_KEY_P: genericKey = Input::Key::P; break;
        case GLFW_KEY_Q: genericKey = Input::Key::Q; break;
        case GLFW_KEY_R: genericKey = Input::Key::R; break;
        case GLFW_KEY_S: genericKey = Input::Key::S; break;
        case GLFW_KEY_T: genericKey = Input::Key::T; break;
        case GLFW_KEY_U: genericKey = Input::Key::U; break;
        case GLFW_KEY_V: genericKey = Input::Key::V; break;
        case GLFW_KEY_W: genericKey = Input::Key::W; break;
        case GLFW_KEY_X: genericKey = Input::Key::X; break;
        case GLFW_KEY_Y: genericKey = Input::Key::Y; break;
        case GLFW_KEY_Z: genericKey = Input::Key::Z; break;

        // Numbers
        case GLFW_KEY_0: genericKey = Input::Key::Num0; break;
        case GLFW_KEY_1: genericKey = Input::Key::Num1; break;
        case GLFW_KEY_2: genericKey = Input::Key::Num2; break;
        case GLFW_KEY_3: genericKey = Input::Key::Num3; break;
        case GLFW_KEY_4: genericKey = Input::Key::Num4; break;
        case GLFW_KEY_5: genericKey = Input::Key::Num5; break;
        case GLFW_KEY_6: genericKey = Input::Key::Num6; break;
        case GLFW_KEY_7: genericKey = Input::Key::Num7; break;
        case GLFW_KEY_8: genericKey = Input::Key::Num8; break;
        case GLFW_KEY_9: genericKey = Input::Key::Num9; break;

        // Whitespace & System
        case GLFW_KEY_SPACE:     genericKey = Input::Key::Space; break;
        case GLFW_KEY_ENTER:     genericKey = Input::Key::Enter; break;
        case GLFW_KEY_ESCAPE:    genericKey = Input::Key::Escape; break;
        case GLFW_KEY_TAB:       genericKey = Input::Key::Tab; break;
        case GLFW_KEY_BACKSPACE: genericKey = Input::Key::Backspace; break;
        case GLFW_KEY_DELETE:    genericKey = Input::Key::Delete; break;

        // Modifiers
        case GLFW_KEY_LEFT_SHIFT:    genericKey = Input::Key::LeftShift; break;
        case GLFW_KEY_RIGHT_SHIFT:   genericKey = Input::Key::RightShift; break;
        case GLFW_KEY_LEFT_CONTROL:  genericKey = Input::Key::LeftControl; break;
        case GLFW_KEY_RIGHT_CONTROL: genericKey = Input::Key::RightControl; break;
        case GLFW_KEY_LEFT_ALT:      genericKey = Input::Key::LeftAlt; break;
        case GLFW_KEY_RIGHT_ALT:     genericKey = Input::Key::RightAlt; break;

        // Navigation
        case GLFW_KEY_UP:    genericKey = Input::Key::ArrowUp; break;
        case GLFW_KEY_DOWN:  genericKey = Input::Key::ArrowDown; break;
        case GLFW_KEY_LEFT:  genericKey = Input::Key::ArrowLeft; break;
        case GLFW_KEY_RIGHT: genericKey = Input::Key::ArrowRight; break;
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
        case GLFW_MOUSE_BUTTON_LEFT:      genericKey = Input::Key::MouseLeft;  break;
        case GLFW_MOUSE_BUTTON_RIGHT:      genericKey = Input::Key::MouseRight; break;
        case GLFW_MOUSE_BUTTON_MIDDLE:    genericKey = Input::Key::MouseMiddle; break;
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
    static bool isFirstFrame = true;

    if(isFirstFrame){
        
        plt->inputState.lastX = xpos;
        plt->inputState.lastY = ypos;
        isFirstFrame = false;
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

