#include "engine.hpp"

using Particle = physics::Particle;


typedef IScript* (*CreateScriptFunc)();

void Engine::run(){

    initialize(); // basically all parts of the engine

    ast.addUploadRequest("../../../../models/dragon.glb");
    ast.addUploadRequest("../../../../models/cube_gltf.glb"); //THESE SHOULD BE DONE IN INITGAME
    
    Scene scn{};
    
    reg.createPool<Particle>();
    reg.createPool<Transform>();
    reg.createPool<Renderable>();
    reg.createPool<Camera>();
    reg.createPool<Script>();
    int cam0 = reg.createEntity();
	reg.emplace<Camera>(cam0);
 
    EngineMode mode = EngineMode::GAME;
    HMODULE GameModule{};
    HMODULE EditorUIHandle{};
    ctx = GameContext{};

    
    const char* factoryName{}; 
    HMODULE module{};
    if (mode == EngineMode::GAME){
        GameModule = loader.loadDLL("../../../../games/tetris/Debug/TetrisDLL.dll");
        factoryName = "GetGameScripts";
    }
   
    /*which mode load correct dll*/
    
    
    std::vector<IScript*> IScripts{};
    
    loader.getGameContextPtr(GameModule, factoryName)(IScripts);
    
    std::vector<Script> scripts{};
    scripts.resize(1);
    scripts[0].ptr = IScripts[0];

    scripts[0].ptr->init(reg, ctx);

    while (plt.windowOpen()) {
        //check 




        plt.updateState(); // update keyboard and dt
        /* pass plt and reg to dll  */
        scripts[0].ptr->update(plt.aspectRatio, plt.deltaTime, plt.inputState, reg, ctx);
        //reg.getPool<Script>().data[0].ptr->update(plt.aspectRatio, plt.deltaTime, plt.inputState, reg);
        plt.inputState.clearCursorDeltas();
        fileWatcher.checkDirectoryPeriodically();
        

        auto [transPool, particPool, rendPool] = reg.getPools<Transform, Particle, Renderable>();

            /*scuffed PHYSICS*/
            for (int i{}; i < particPool.count; i++){
                ECS::Entity e = particPool.dense[i];
                Particle& p = particPool.data[i];
                p.integrate(plt.deltaTime,true);
                transPool.get(e).position = p.pos;
            // std::cout << "VELOCITY of entity " << (int)e << ": " <<  p.vel.x << ", " <<  p.vel.y << ", " <<  p.vel.z << "\n";
            }

            

            /*FILL renderobjects array*/
            scn.gameObjects.resize((uint32_t)rendPool.count);
            for (int i{}; i < rendPool.count; i++){
                ECS::Entity e = rendPool.dense[i];
                const Renderable& rend = rendPool.data[i];
                scn.gameObjects[i].meshID = rend.meshID;
                scn.gameObjects[i].model = transPool.get(e).matrix();
                
            }
            
            
            /*RENDERER */
            if (stk.acquireAndValidateImage(plt)){
                stk.startFrame();
                stk.flushRequests(ast.requests, ast.storage);
                auto& cam = reg.getPool<Camera>().get(cam0);
                stk.updateUBO(cam.view, cam.proj);
                
                stk.render(scn, ast.storage); //pass gameobjs,   



                if(true){
                    edt.render(stk.cmdBuffers[stk.currentFrame], ctx);
                }
               

                stk.endFrame();
            }
   
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
    stk.initUpdateDescriptorSets();
    
    edt.init(stk,plt);

    stk.initPhongPipeline(
        shaderCompiler.compileFile("phong_vert", shaderc_vertex_shader, fileReader.readFile("../../../../src/shaders/phong.vert"),true),
        shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("../../../../src/shaders/phong.frag"),true)
    );
    
    
    fileWatcher.setCheckTime(5);
    fileWatcher.setFileDirectory("../../../../src/shaders");
    fileWatcher.warmupDirectory();
    fileWatcher.setStandardResponse();

}
