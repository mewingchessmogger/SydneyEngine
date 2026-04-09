#include "engine.hpp"

void Engine::run(){
    initialize();
    ast.addUploadRequest("../../../../models/dragon.glb");
    ast.addUploadRequest("../../../../models/cube_gltf.glb");
    
    Scene scn{};
    while (plt.windowOpen()) {
        
        plt.inputState.previous = plt.inputState.current;
        plt.pollEvents();
        plt.calculateDeltaTime();
        updateGame(scn, plt.getWindowAspect(), plt.inputState, ast.storage);
        plt.inputState.clearCursorDeltas();
        
        fileWatcher.checkDirectoryPeriodically();
        if (stk.acquireAndValidateImage(plt)){
            stk.startFrame();
            stk.flushTransferBuffer(ast.requests, ast.storage);
            
            stk.updateUBO(scn.data);
            stk.render(scn, ast.storage); //pass gameobjs,   
            stk.endFrame();
        }

        
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