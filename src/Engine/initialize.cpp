#include "engine.hpp"
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
    
    edt.init(stk.ctx,plt, stk.res.swapchainImages[0].format);
    printf("Processing pipelines ");


    printf("1.. ");
    stk.initPhongPipeline(
        std::move(shaderCompiler.compileFile("phong_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/phong.vert"),true)),
        std::move(shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/phong.frag"),true))
    );
    printf("2..? ");
    stk.initSkinPhongPipeline(
        std::move(shaderCompiler.compileFile("phong_skin_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/phong_skin.vert"),true)),
        std::move(shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/phong_skin.frag"),true))
    );

    printf("3....!?? ");
    stk.initColliderPipeline(
        std::move(shaderCompiler.compileFile("phong_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/collider.vert"),true)),
        std::move(shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/collider.frag"),true))
    );
    printf("Done!\n");

    fileWatcher.setCheckTime(5);
    fileWatcher.setFileDirectory("src/shaders");
    fileWatcher.warmupDirectory();
    fileWatcher.setStandardResponse();

}