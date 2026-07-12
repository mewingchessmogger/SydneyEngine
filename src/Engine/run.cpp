#include "engine.hpp"
using Particle = physics::Particle;
void Engine::run(){

    
    initialize(); // basically all parts of the engine
    //models/shibahu.glb
    // ast.addUploadRequest("models/dragon.glb");
    // ast.addUploadRequest("models/cube_gltf.glb"); //THESE SHOULD BE DONE IN INITGAME
   
    ldr.loadScene("models/cube_gltf.glb");//static
    ldr.loadScene("models/dragon.glb"); //static
    std::string path = "models/shibahu.glb";
    const aiScene* skinScn = ldr.getScene(path);
    ldr.loadScene(skinScn, path);
    ldr.scenes.push_back(skinScn);

    std::vector<RenderPkt> packets{};    
    reg.createPool<Particle>();
    reg.createPool<Transform>();
    reg.createPool<Renderable>();
    reg.createPool<Camera>();

    int gameCamID = reg.createEntity();
    int editorCamID = reg.createEntity();
	reg.emplace<Camera>(gameCamID);
    reg.emplace<Camera>(editorCamID);

    EngineMode mode = EngineMode::EDITOR;
    HMODULE GameModule = loader.loadDLL("games/tetris/Debug/TetrisDLL.dll");
    ctx = GameContext{};

    std::vector<IScript*> IScripts{};
    
    loader.getGameContextPtr(GameModule, "GetGameScripts")(IScripts);
    
    Script game{};
    game.ptr = IScripts[0];
    game.ptr->init(reg, ctx);



    while (plt.windowOpen()) {
        plt.updateState(); // update keyboard and dt

        if(plt.inputState.keyPressed(Input::Key::Escape)){
            mode = (mode == EngineMode::GAME) ? EngineMode::EDITOR : EngineMode::GAME;
            if (mode == EngineMode::GAME){
                plt.inputState.requestCursorVisible = false;    //no biggie just doing it also here cuz sometimes fails to switch
            }
            if (mode == EngineMode::EDITOR){
                plt.inputState.requestCursorVisible = true;    //no biggie just doing it also here cuz sometimes fails to switch
            }
            
        }
        Camera& activeCam = reg.getPool<Camera>().get((mode == EngineMode::GAME) ? gameCamID : editorCamID);

        if (mode == EngineMode::GAME){         
            updateCamera(activeCam,EngineMode::GAME);
            game.ptr->update(plt.aspectRatio, plt.deltaTime, plt.inputState, reg, ctx);
            updatePhysics();
        }
        
        
        fileWatcher.checkDirectoryPeriodically();
        
        prepareRenderables(packets);
        
        if (stk.acquireAndValidateImage(plt))
        {
            edt.evalViewport(stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)],stk.res.viewportImages); //required convoluted mess for my imgui setup to work 
            stk.startFrame();
            if (stk.flushUploads(ldr.getAssetReg())){
                stk.endFrame();// TODO fix validation error 
                continue;
            };
            AssetRegistry::SkinnedModel& mdl = ldr.getAssetReg().getSkinnedModelFromID(2);

            stk.updateUBO(activeCam.view, activeCam.proj,mdl.finalBoneMatrices);
            stk.render(packets, ldr.getAssetReg());
            
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