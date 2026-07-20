#include "pch.hpp"
using Particle = physics::Particle;
void Engine::run(){
    initialize(); // basically all parts of the engine
    
    reg.createPool<Particle>();
    reg.createPool<Transform>();
    reg.createPool<Renderable>();
    reg.createPool<Camera>();
    reg.createPool<Animated>();
    
    int editorCamID = reg.createEntity();
    reg.emplace<Camera>(editorCamID);
    
    
    EngineMode mode = EngineMode::GAME;
    
    std::vector<IScript*> IScripts{};    
    HMODULE GameModule = loader.loadDLL("games/tetris/Debug/TetrisDLL.dll");
    loader.getGameContextPtr(GameModule, "GetGameScripts")(IScripts);
    
    Script game{};
    game.ptr = IScripts[0];
    game.ptr->init(reg, api);


    
    while (plt.windowOpen()) {
        plt.updateState(); // update keyboard and dt
        if(plt.inputState.keyPressed(Input::Key::Escape)){
            mode = (mode == EngineMode::GAME) ? EngineMode::EDITOR : EngineMode::GAME;
            plt.inputState.requestCursorVisible = (mode == EngineMode::EDITOR);
        }
        if(plt.inputState.keyPressed(Input::Key::Quit)){
            break;
        }

        Camera& activeCam = reg.getPool<Camera>().get((mode == EngineMode::GAME) ? api.getGameCamera() : editorCamID);

        if (mode == EngineMode::GAME){         
            game.ptr->update(plt.aspectRatio, plt.deltaTime, plt.inputState, reg, api);
            updatePhysics();
        }
        
        processAPI(); // api.setAnimation(gameID, "shooting_ak");

        updateAnimations(plt.deltaTime);
        
        prepareRenderables(stk.packets);

        if (stk.acquireAndValidateImage(plt))
        {
            edt.evalViewport(stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)],stk.res.viewportImages); //required convoluted mess for my imgui setup to work 
            stk.startFrame();
            if (stk.flushUploads(ldr.getAssetReg())){
                stk.abortFrame();
                continue;
            };
            
            stk.updateUBO(activeCam.view, activeCam.proj);
            
            stk.render(ldr.getAssetReg());
            
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
}