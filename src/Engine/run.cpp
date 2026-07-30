#include "engine.hpp"
#include "passed_structures_dll.hpp"
#define CR_HOST CR_SAFE
#include "cr.h"
#include "systems.hpp"
using Particle = physics::Particle;
void Engine::run(){
    initialize(); // basically all parts of the engine
    
    reg.createPool<Particle>();
    reg.createPool<Transform>();
    reg.createPool<Renderable>();
    reg.createPool<Camera>();
    reg.createPool<Animated>();
    reg.createPool<Node>();
    int editorCamID = reg.createEntity();
    reg.emplace<Camera>(editorCamID);
    
    EngineMode mode = EngineMode::GAME;
    PassedStructuresDLL psd = {.reg = &reg, .api = &api, .state = &plt.inputState, .aspect = &plt.aspectRatio, .dt = & plt.deltaTime};
    cr_plugin game_plugin = {.userdata = &psd};
    cr_plugin_open(game_plugin, "games/tetris/Debug/TetrisDLL.dll"); 
    // plugin physics
    // plugin renderer
    // plugin sound
    // plugin 

    while (plt.windowOpen()) {
        plt.updateState(); // update keyboard and dt
        if(plt.inputState.keyPressed(Input::Key::Escape)){
            mode = (mode == EngineMode::GAME) ? EngineMode::EDITOR : EngineMode::GAME;
            plt.inputState.requestCursorVisible = (mode == EngineMode::EDITOR);
        }
        if(plt.inputState.keyPressed(Input::Key::L)){
            break;
        }

        Camera& activeCam = reg.getPool<Camera>().get((mode == EngineMode::GAME) ? api.getGameCamera() : editorCamID);
        
        
        
        if (mode == EngineMode::GAME){        
            cr_plugin_update(game_plugin);
            Sys::updatePhysics(reg, plt.deltaTime);
        }
        

        //processAPI();
        Sys::processAPI(reg, ldr.getAssetReg(), api, ldr);
        Sys::propagateNodes(reg);
        Sys::updateAnimations(reg, plt.deltaTime);
        Sys::buildRenderPackets(reg, ldr.getAssetReg(), stk.packets);
        
        
        if (stk.acquireAndValidateImage(PlatformGLFW::stallMinimizedWindow, plt.windowPtr, plt.glwidth, plt.glheight, plt.frameBufferResized, plt.aspectRatio))
        {
            

            edt.evalViewport(stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)], stk.res.viewportImages); //required convoluted mess for my imgui setup to work 
            stk.startFrame();
            if (stk.flushUploads(ldr.getAssetReg())){
                //stk.abortFrame  (); //laptop conected to monitor crashes on this, not on laptop screen only
                //continue;
            };
            
            stk.updateUBO(activeCam.view, activeCam.proj);
            
            stk.render();
            
            if(mode == EngineMode::EDITOR){
                stk.blitTargetToViewport(); //viewport in imgui
                stk.startEditorToSwapchain();
                edt.messingAround(stk.cmdBuffers[stk.currentFrame], reg, ctx, stk.currentImgIndex, activeCam,plt);// IMGUI WILL CREATE OWN RENDER PASS IF OUTSIDE GLFW WINDOW
                stk.endEditorToSwapchain();
                edt.updateEditorInput();
            }
            else if(mode == EngineMode::GAME){
                stk.blitTargetToSwapchain();
            }
            stk.endFrame();
        }
              
        plt.inputState.clearCursorDeltas();
    }
    plt.shutdown();
    cr_plugin_close(game_plugin);
}