#include "engine.hpp"
#include "passed_structures_dll.hpp"
#define CR_HOST CR_SAFE
#include "cr.h"

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
    api.loadModel("models/cube_gltf.glb");
    
    EngineMode mode = EngineMode::GAME;
    PassedStructuresDLL psd = {.reg = &reg, .api = &api, .state = &plt.inputState, .aspect = &plt.aspectRatio, .dt = & plt.deltaTime};
    cr_plugin cr_ctx = {.userdata = &psd};
    cr_plugin_open(cr_ctx, "games/tetris/Debug/TetrisDLL.dll"); 


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
            cr_plugin_update(cr_ctx);
            updatePhysics();
        }
        

        processAPI();
        updateAnimations(plt.deltaTime);
        propagateNodes();
        prepareRenderables(stk.packets);
        std::vector<int> foo{};
        
        if (stk.acquireAndValidateImage(plt))
        {
            edt.evalViewport(stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)],stk.res.viewportImages); //required convoluted mess for my imgui setup to work 
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
    cr_plugin_close(cr_ctx);
}