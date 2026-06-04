#include "engine.hpp"

using Particle = physics::Particle;


typedef IScript* (*CreateScriptFunc)();

// void loadAndRegisterTetris(ECS::Registry& reg) {
//     // 1. Load the compiled game binary from the runtime folder
//     HMODULE dllHandle = LoadLibraryA("../../../../games/tetris/Debug/TetrisDLL.dll");
//     if (!dllHandle) {
//         std::cerr << "Failed to load Tetris DLL\n";
//         return;
//     }

//     // 2. Get the address of the factory function
//     CreateScriptFunc createInstance = (CreateScriptFunc)GetProcAddress(dllHandle, "CreateScriptInstance");
//     if (!createInstance) {
//         std::cerr << "Failed to locate factory function\n";
//         FreeLibrary(dllHandle);
//         return;
//     }

//     // 3. Instantiate the Tetris class across the DLL boundary
//     IScript* rawGamePointer = createInstance();

//     // 4. Wrap it in a unique_ptr and create the component container
//     Script scriptComponent;
//     scriptComponent.ptr = std::unique_ptr<IScript>(rawGamePointer);

//     // 5. Initialize the game logic module
//     scriptComponent.ptr->init(reg);

//     // 6. Create an entity and push the script component straight into the pool
//     // (Uses your upgraded move-only perfect forwarding add function)
//     int tetrisEntity = reg.createEntity();
//     reg.add(tetrisEntity, std::move(scriptComponent));
// }



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
    
    Script script{};
    script.ptr = loader.acquireScriptPtr(loader.loadGameDLL("../../../../games/tetris/Debug/TetrisDLL.dll"),"CreateScriptInstance");
    script.ptr->init(reg);

    while (plt.windowOpen()) {
        //check 


        plt.updateState(); // update keyboard and dt

        script.ptr->update(plt.aspectRatio, plt.deltaTime, plt.inputState, reg);
        //reg.getPool<Script>().data[0].ptr->update(plt.aspectRatio, plt.deltaTime, plt.inputState, reg);
        
        
        auto [transPool, particPool, rendPool] = reg.getPools<Transform, Particle, Renderable>();

        for (int i{}; i < particPool.count; i++){
            ECS::Entity e = particPool.dense[i];
            Particle& p = particPool.data[i];
            p.integrate(plt.deltaTime,true);
            transPool.get(e).position = p.pos;
          // std::cout << "VELOCITY of entity " << (int)e << ": " <<  p.vel.x << ", " <<  p.vel.y << ", " <<  p.vel.z << "\n";
        }

        
        scn.gameObjects.resize((uint32_t)rendPool.count);

        for (int i{}; i < rendPool.count; i++){
            ECS::Entity e = rendPool.dense[i];
            const Renderable& rend = rendPool.data[i];
            scn.gameObjects[i].meshID = rend.meshID;
            scn.gameObjects[i].model = transPool.get(e).matrix();
            
        }
        
        

        plt.inputState.clearCursorDeltas();
        fileWatcher.checkDirectoryPeriodically();
        
        if (stk.acquireAndValidateImage(plt)){
            stk.startFrame();
            stk.flushRequests(ast.requests, ast.storage);
            auto& cam = reg.getPool<Camera>().get(cam0);
            stk.updateUBO(cam.view, cam.proj);
            
            stk.render(scn, ast.storage); //pass gameobjs,   
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
    
    stk.initPhongPipeline(
        shaderCompiler.compileFile("phong_vert", shaderc_vertex_shader, fileReader.readFile("../../../../src/shaders/phong.vert"),true),
        shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("../../../../src/shaders/phong.frag"),true)
    );
    
    
    fileWatcher.setCheckTime(5);
    fileWatcher.setFileDirectory("../../../../src/shaders");
    fileWatcher.warmupDirectory();
    fileWatcher.setStandardResponse();
}
