#include "engine.hpp"
#include "game.hpp"

void Engine::run(){

    initialize(); // basically all parts of the engine

    ast.addUploadRequest("../../../../models/dragon.glb");
    ast.addUploadRequest("../../../../models/cube_gltf.glb"); //THESE SHOULD BE DONE IN INITGAME
    
    Scene scn{};
    
    reg.createPool<Particle>();
    reg.createPool<Transform>();
    reg.createPool<Renderable>();
    initGame(scn,reg);

    while (plt.windowOpen()) {
        plt.updateState(); // update keyboard and dt

        updateGame(scn, plt.aspectRatio, plt.deltaTime, plt.inputState, ast.storage, reg);
        

        
        auto& TPool = reg.getPool<Transform>();
        auto& PPool = reg.getPool<Particle>();
        auto& RPool = reg.getPool<Renderable>();
        
        for (int i{}; i < PPool.count; i++){
            ECS::Entity e = PPool.dense[i];
            Particle& p = PPool.data[i];
            p.integrate(plt.deltaTime,true);
            TPool.get(e).position = p.pos;
            std::cout << "VELOCITY of entity " << (int)e << ": " <<  p.vel.x << "\n";
        }


        scn.gameObjects.resize((uint32_t)RPool.count);

        for (int i{}; i < RPool.count; i++){
            ECS::Entity e = RPool.dense[i];
            const Renderable& rend = RPool.data[i];
            scn.gameObjects[i].meshID = rend.meshID;
            scn.gameObjects[i].model = TPool.get(e).matrix();
            
        }
        
        

        plt.inputState.clearCursorDeltas();
        fileWatcher.checkDirectoryPeriodically();
        
        if (stk.acquireAndValidateImage(plt)){
            stk.startFrame();
            stk.flushRequests(ast.requests, ast.storage);
            
            stk.updateUBO(scn.data);
            
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
