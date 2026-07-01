#include "engine.hpp"
#include "asset_loader.hpp"


using Particle = physics::Particle;

void Engine::run(){

    
    initialize(); // basically all parts of the engine
    //models/shibahu.glb
    // ast.addUploadRequest("models/dragon.glb");
    // ast.addUploadRequest("models/cube_gltf.glb"); //THESE SHOULD BE DONE IN INITGAME
    AssetLoader ldr{};
   
    //ldr.loadScene("models/cube_gltf.glb");
    ldr.loadScene("models/dragon.glb");
    Scene scn{};
    
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
        /* pass plt and reg to dll  */
        if(plt.inputState.keyPressed(Input::Key::Escape)){
            mode = (mode == EngineMode::GAME) ? EngineMode::EDITOR : EngineMode::GAME;
            if (mode == EngineMode::GAME){
                plt.inputState.requestCursorVisible = false;    
            }
        }
        Camera& activeCam = reg.getPool<Camera>().get((mode == EngineMode::GAME) ? gameCamID : editorCamID);

        if (mode == EngineMode::GAME){         
            updateCamera(activeCam,EngineMode::GAME);
            game.ptr->update(plt.aspectRatio, plt.deltaTime, plt.inputState, reg, ctx);
            updatePhysics();
        }

        
        fileWatcher.checkDirectoryPeriodically();
        
        prepareRenderables(scn);
        //stk.fillRenderQueue(reg, ldr.getAssetReg());
        
        if (stk.acquireAndValidateImage(plt))
        {
            edt.evalViewport(stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)],stk.res.viewportImages);
            stk.startFrame();
            //stk.flushRequests(ast.requests, ast.storage);
            stk.flushRequests(ldr.getAssetReg());
            stk.updateUBO(activeCam.view, activeCam.proj);
            stk.render(scn, ldr.getAssetReg());
            //begin render asss

            if(mode == EngineMode::EDITOR){
                stk.blitTargetToViewport();
                stk.startEditorToSwapchain();
                edt.messingAround(stk.cmdBuffers[stk.currentFrame], reg, ctx, stk.currentImgIndex, activeCam,plt);// IMGUI WILL CREATE OWN RENDER PASS IF OUTSIDE GLFW WINDOW
                stk.endEditorToSwapchain();
                edt.updateEditorInput();
            }
            else{
                stk.blitTargetToSwapchain();
            }
            
            stk.endFrame();
        }
         
        
             
        plt.inputState.clearCursorDeltas();

        
    }
    plt.shutdown();
}

//transform, 
void Engine::initialize(){
 
    plt.initWindow(stk.WIDTH,stk.HEIGHT);
    stk.initInstance(plt);
    stk.initDevice(plt);
    stk.initSyncs();
    stk.initCommands();
    stk.initDescriptorStuff();
    stk.initBuffers();
    stk.initSwapchain();
    stk.initDepthImages();
    stk.initRenderTargetImages();
    stk.initViewportImages();
    stk.initColorPickImage();
    stk.initUpdateDescriptorSets();
    
    edt.init(stk,plt);
    std::cout << "Processing phong...";


    stk.initPhongPipeline(
        shaderCompiler.compileFile("phong_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/phong.vert"),true),
        shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/phong.frag"),true)
    );

    std::cout << "Done!\n";

    // std::cout << "Processing pick...";
    // stk.initPickPipeline(
    //     shaderCompiler.compileFile("pick_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/pick.vert"),true),
    //     shaderCompiler.compileFile("pick_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/pick.frag"),true)
    // );
    //     std::cout << "Done!\n";

    
    fileWatcher.setCheckTime(5);
    fileWatcher.setFileDirectory("src/shaders");
    fileWatcher.warmupDirectory();
    fileWatcher.setStandardResponse();

}





void Engine::updatePhysics()
{
        /*you get a copy of vector filled with refs*/
        auto [transPool, particPool] = reg.getPools<Transform, Particle>();
        /*scuffed PHYSICS*/
        for (int i{}; i < particPool.count; i++){
            ECS::Entity e = particPool.dense[i];
            Particle& p = particPool.data[i];
            p.integrate(plt.deltaTime,true);
            transPool.get(e).position = p.pos;
            // std::cout << "VELOCITY of entity " << (int)e << ": " <<  p.vel.x << ", " <<  p.vel.y << ", " <<  p.vel.z << "\n";
        }
}

void Engine::prepareRenderables(Scene& scn)
{
    auto [transPool, rendPool] = reg.getPools<Transform, Renderable>();


    /*FILL renderobjects array*/
    scn.gameObjects.resize((uint32_t)rendPool.count);
    for (int i{}; i < rendPool.count; i++){
        ECS::Entity e = rendPool.dense[i];
        const Renderable& rend = rendPool.data[i];
        scn.gameObjects[i].meshID = rend.meshID;
        scn.gameObjects[i].model = transPool.get(e).matrix();
        
    }
}

