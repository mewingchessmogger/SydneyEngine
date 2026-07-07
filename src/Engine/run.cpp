#include "engine.hpp"
#include "asset_loader.hpp"


using Particle = physics::Particle;

void Engine::run(){

    
    initialize(); // basically all parts of the engine
    //models/shibahu.glb
    // ast.addUploadRequest("models/dragon.glb");
    // ast.addUploadRequest("models/cube_gltf.glb"); //THESE SHOULD BE DONE IN INITGAME
    AssetLoader ldr{};
   
    ldr.loadScene("models/cube_gltf.glb");
    ldr.loadScene("models/dragon.glb");
    ldr.loadScene("models/shibahu.glb");
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

        if(plt.inputState.keyPressed(Input::Key::Escape)){
            mode = (mode == EngineMode::GAME) ? EngineMode::EDITOR : EngineMode::GAME;
            if (mode == EngineMode::GAME){
                plt.inputState.requestCursorVisible = false;    //no biggie just doing it also here cuz sometimes fails to switch
            }
        }
        Camera& activeCam = reg.getPool<Camera>().get((mode == EngineMode::GAME) ? gameCamID : editorCamID);

        if (mode == EngineMode::GAME){         
            updateCamera(activeCam,EngineMode::GAME);
            game.ptr->update(plt.aspectRatio, plt.deltaTime, plt.inputState, reg, ctx);
            updatePhysics();
        }
        
        
        fileWatcher.checkDirectoryPeriodically();
        
        prepareRenderables(scn, ldr.getAssetReg());
        
        if (stk.acquireAndValidateImage(plt))
        {
            edt.evalViewport(stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)],stk.res.viewportImages); //required convoluted mess for my imgui setup to work 
            stk.startFrame();
            if (stk.flushUploads(ldr.getAssetReg())){
                stk.endFrame();
                continue;
            };
            stk.updateUBO(activeCam.view, activeCam.proj);
            stk.render(scn.packets, ldr.getAssetReg());
            

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
    stk.initSkinPhongPipeline(
        shaderCompiler.compileFile("phong_skin_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/phong_skin.vert"),true),
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

void Engine::prepareRenderables(Scene& scn, AssetRegistry& astReg)
{
    auto [transPool, rendPool] = reg.getPools<Transform, Renderable>();

    scn.packets.clear();

    /*FILL renderobjects array*/
    for (int i{}; i < rendPool.count; i++){
        ECS::Entity e = rendPool.dense[i];
        const Renderable& rend = rendPool.data[i];
        
        if(astReg.isSkinned(rend.id)){
            
            
            AssetRegistry::SkinnedModel& mdl = astReg.getSkinnedModelFromID(rend.id);
            printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesSkinnedVBO/sizeof(SkinnedVertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
            glm::mat4 modelMat = transPool.get(e).matrix() * mdl.normalizeMat;
            Scene::RenderPkt pkt{};
            pkt.pc.modelSpace = modelMat;
            pkt.type = Scene::Mesh::SKINNED;
            pkt.pc.offsetVBO = mdl.baseOffsetBytesSkinnedVBO /sizeof(SkinnedVertex);

            for(auto& mesh : mdl.meshes){
                pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t); //  global
                
                pkt.indexCount = mesh.indexCount;
                pkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                scn.packets.push_back(pkt);
            }
        }
        else{
            
            AssetRegistry::StaticModel& mdl = astReg.getStaticModelFromID(rend.id);
            printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesVBO/sizeof(Vertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
            glm::mat4 modelMat = transPool.get(e).matrix() * mdl.normalizeMat;
            Scene::RenderPkt pkt{};
            pkt.pc.modelSpace = modelMat;
            pkt.type = Scene::Mesh::STATIC;
            pkt.pc.offsetVBO = mdl.baseOffsetBytesVBO / sizeof(Vertex);

            for(auto& mesh : mdl.meshes){
                pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t); //  global
                pkt.indexCount = mesh.indexCount;
                pkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                scn.packets.push_back(pkt);
            }
        }


    }


}

