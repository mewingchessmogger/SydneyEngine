#include "engine.hpp"
#include "asset_loader.hpp"

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
        shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("src/shaders/phong_skin.frag"),true)
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