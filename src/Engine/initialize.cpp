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
    printf("Processing phong...");


    stk.initPhongPipeline(
        std::move(shaderCompiler.compileFile("phong_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/phong.vert"),true)),
        std::move(shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/phong.frag"),true))
    );
    stk.initSkinPhongPipeline(
        std::move(shaderCompiler.compileFile("phong_skin_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/phong_skin.vert"),true)),
        std::move(shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/phong_skin.frag"),true))
    );
    stk.initSkinPhongPipeline(
        std::move(shaderCompiler.compileFile("phong_skin_vert", shaderc_vertex_shader, fileReader.readFile("src/shaders/phong_skin.vert"),true)),
        std::move(shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/phong_skin.frag"),true))
    );

    printf("Done!\n");

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