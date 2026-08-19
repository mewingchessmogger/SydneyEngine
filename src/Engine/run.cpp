#include "engine.hpp"
#include "passed_structures_dll.hpp"
#define CR_HOST CR_SAFE
#include "cr.h"
#include "systems.hpp"

void Engine::run(){
    initialize(); // basically all parts of the engine
    
    int editorCamID = reg.createEntity();
    reg.emplace<Camera>(editorCamID);
    
    EngineMode mode = EngineMode::GAME;
    RawMemory mem(1024*1024); // 1024 kilobytes
    PassedStructuresDLL psd = {.reg = &reg, .api = &api, .state = &plt.inputState, .aspect = &plt.aspectRatio, .dt = & plt.deltaTime, .mem = &mem};
    cr_plugin game_plugin = {.userdata = &psd};
    cr_plugin_open(game_plugin, "games/tetris/Debug/TetrisDLL.dll"); 
    // plugin physics
    // plugin renderer
    // plugin sound
    // plugin 

    while (plt.windowOpen()) {
        plt.updateState(); // update keyboard and dt 
        bool shutDown = debugStuff(plt, mode, ldr.getAssetReg()); if(shutDown) { break;}

        Camera& activeCam = reg.getPool<Camera>().get((mode == EngineMode::GAME) ? api.getGameCamera() : editorCamID);
        
        //https://stackoverflow.com/questions/1055452/c-get-name-of-type-in-template
        
        if (mode == EngineMode::GAME){        
            cr_plugin_update(game_plugin);
            Sys::updatePhysics(reg, plt.deltaTime);
            //physics.integrate();
            //physics.gjk();
            //physics.epa();
            //physics.sat();
            
            //Sys::Integrate(reg,dt);
            

           // -> trans + info + colliders  -> [physics] -> events + collision resolutions  
            
            
            

        }
        

        Sys::processAPI(reg, ldr.getAssetReg(), api, ldr); //A LOT HAPPENS IN HERE
        reg.updateHierarchyLevels();
        reg.sortHierarchyPool();
        Sys::finalizeTransforms(reg);
        Sys::updateAnimations(reg, plt.deltaTime);
        Sys::buildRenderPackets(reg, ldr.getAssetReg(), stk.packets);  
        
        /*buncha stuff rebuilt inside validateImage if window is resized including imgui stuff, */
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
                //stk.blit(Src::TARGET, Dst::EDITOR_VIEWPORT)
                stk.blitTargetToViewport(); //viewport in imgui
                stk.startEditorToSwapchain();
                edt.messingAround(stk.cmdBuffers[stk.currentFrame], reg, ctx, stk.currentImgIndex, activeCam,plt);// IMGUI WILL CREA OWN RENDER PASS IF OUTSIDE GLFW WINDOW
                stk.endEditorToSwapchain();
                edt.updateEditorInput();
            }
            else if(mode == EngineMode::GAME){
                //stk.blit(Src::TARGET, Dst::SWAPCHAIN)
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