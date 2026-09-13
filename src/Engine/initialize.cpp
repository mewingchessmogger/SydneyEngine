#include "engine.hpp"
void Engine::initialize(){
    //glfw stuff
    plt.initWindow(stk.WIDTH,stk.HEIGHT);


    //vulkan setting up buffers and other vulkan host states
    stk.initInstance(PlatformGLFW::getInstanceExtensions);
    stk.initDevice(plt.windowPtr, PlatformGLFW::createWindowSurface);
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
    
    //editor setup
    edt.init(stk.ctx,plt, stk.res.swapchainImages[0].format);
    printf("Processing pipelines ");

    //pipelines compilation, funcs stripped from shaderc examples
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



    /*not used at all 4 loc below*/
    // fileWatcher.setCheckTime(5);
    // fileWatcher.setFileDirectory("src/shaders");
    // fileWatcher.warmupDirectory();  
    // fileWatcher.setStandardResponse();// very much unused here
    
    //creating Ecs Pools that engine completely knows about, game can create its own that engine dont know about, all components need COMP_NAME macro to function
    // shitty ways of doing same funcitonalty as my comp_name macro -> https://stackoverflow.com/questions/1055452/c-get-name-of-type-in-template

    reg.createPool<SydX::Particle>();
    reg.createPool<TransformInfo>();
    reg.createPool<RawTransform>();
    reg.createPool<Collider>();
    reg.createPool<Renderable>();
    reg.createPool<Camera>();
    reg.createPool<Animated>();
   


}