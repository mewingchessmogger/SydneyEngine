#include "engine.hpp"
void Engine::run(){
    
    plt.initWindow(stk.WIDTH,stk.HEIGHT);
    stk.initInstance(plt);
    stk.initDevice(plt);
    stk.initSyncs();
    stk.initCommands();
    stk.initDescriptorStuff();
    stk.initBuffers();
    stk.initSwapchain();
    
    stk.initTestPipeline(
        shaderCompiler.compileFile("test_vert",shaderc_vertex_shader, fileReader.readFile("../../../../src/shaders/test.vert"),true),
        shaderCompiler.compileFile("test_frag",shaderc_fragment_shader, fileReader.readFile("../../../../src/shaders/test.frag"),true)
    );
    fileWatcher.setStandardResponse();
    while (plt.windowOpen()) {
        plt.pollEvents();
        fileWatcher.checkDirectoryPeriodically();

        //VULKAN
        if (!stk.acquireAndValidateImage(plt)){
            continue;
        }

        stk.startFrame();

        stk.render(); //pass gameobjs, render graph genned from inside

        
        stk.concludeFrame();
    }
    
    plt.shutdown();
}