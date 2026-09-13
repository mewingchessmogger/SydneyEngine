#include "engine.hpp"
#include "passed_structures_dll.hpp"
#define CR_HOST CR_SAFE
#include "cr.h"
#include "systems.hpp"

void Engine::run(){

    initialize(); // basically all parts of the engine, loading assets are done at runtime not in here
    
    int editorCamID = reg.createEntity();
    reg.emplace<Camera>(editorCamID);
    EngineMode mode = EngineMode::GAME;// can either start in game or editor
    
    //planned future is that you can switch out games like a cartridge, cr.h is amazing library used here to enable hot reload DLLs
    RawMemory mem(1024*1024); // 1024 kilobytes
    PassedStructuresDLL psd = {.reg = &reg, .api = &api, .state = &plt.inputState, .aspect = &plt.aspectRatio, .dt = & plt.deltaTime, .mem = &mem};
    cr_plugin game_plugin = {.userdata = &psd}; // we just pass all state we want in dll as a pointer here!
    cr_plugin_open(game_plugin, "games/tetris/Debug/TetrisDLL.dll"); //game plugin contains the pointer! works if DLL shares same def as PassedStructuresDLL

    while (plt.windowOpen()) {
        plt.updateState(); // update keyboard and dt 
        bool shutDown = debugStuff(plt, mode, ldr.getAssetReg()); if(shutDown) { break;}

        Camera& activeCam = reg.getPool<Camera>().get((mode == EngineMode::GAME) ? api.getGameCamera() : editorCamID);
        
        
        if (mode == EngineMode::GAME){        
            cr_plugin_update(game_plugin);
        }
        
       
        Sys::processAPI(reg, ldr.getAssetReg(), api, ldr);  // the way game sends requests to engine via api, api queue can be pushed anywhere aswell, future also in editor?
       
        reg.updateHierarchyLevels(); reg.sortHierarchyPool();

        Sys::finalizeTransforms(reg); 
        Sys::updatePhysics(reg, plt.deltaTime);
        
        Sys::updateAnimations(reg, plt.deltaTime);

        Sys::buildRenderPackets(reg, ldr.getAssetReg(), stk.packets);  
        
        /*THIS IS THE SPLIT BETWEEN ENGINE AND RENDERER, PACKETS ARE GIVEN TO VKSTACK, */

        if (stk.acquireAndValidateImage(PlatformGLFW::stallMinimizedWindow, plt.windowPtr, plt.glwidth, plt.glheight, plt.frameBufferResized, plt.aspectRatio))/*buncha stuff rebuilt inside validateImage if window is resized including imgui stuff, */
        {
            edt.evalViewport(stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)], stk.res.viewportImages); //required convoluted mess for my imgui setup to work 
            stk.startFrame();
            if (stk.flushUploads(ldr.getAssetReg())){
                //stk.abortFrame  (); //laptop conected to monitor crashes on this, not on laptop screen only
                //continue;
            };
            
            stk.updateUBO(activeCam.view, activeCam.proj);

            stk.renderToTarget();

            if(mode == EngineMode::EDITOR){
                stk.blitTargetToViewport(); //viewport in imgui
                stk.startEditorToSwapchain();
                edt.messingAround(stk.cmdBuffers[stk.currentFrame], reg, ctx, stk.currentImgIndex, activeCam,plt);// IMGUI WILL CREA OWN RENDER PASS IF OUTSIDE GLFW WINDOW
                stk.endEditorToSwapchain();
                edt.updateEditorInput();
            }
            else if(mode == EngineMode::GAME){
                //edt.renderUI()
                stk.blitTargetToSwapchain();
            }
            stk.endFrame();
        }
              
        plt.inputState.clearCursorDeltas();
    }
    plt.shutdown();
    cr_plugin_close(game_plugin);
}