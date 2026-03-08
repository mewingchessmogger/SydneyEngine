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
    stk.initUpdateDescriptorSets();
    // stk.initTestPipeline(
    //     shaderCompiler.compileFile("test_vert", shaderc_vertex_shader, fileReader.readFile("../../../../src/shaders/test.vert"),true),
    //     shaderCompiler.compileFile("test_frag", shaderc_fragment_shader, fileReader.readFile("../../../../src/shaders/test.frag"),true)
    // );
     
    stk.initPhongPipeline(
        shaderCompiler.compileFile("phong_vert", shaderc_vertex_shader, fileReader.readFile("../../../../src/shaders/phong.vert"),true),
        shaderCompiler.compileFile("phong_frag", shaderc_fragment_shader, fileReader.readFile("../../../../src/shaders/phong.frag"),true)
    );
    
    Scene scn{};

    

    bool addModel = true;
    fileWatcher.setStandardResponse();
    
    while (plt.windowOpen()) {
        plt.pollEvents();
        fileWatcher.checkDirectoryPeriodically();
        
        if (!stk.acquireAndValidateImage(plt)){
            continue;
        }

        stk.startFrame();
            if (addModel){
                addModel = false;
                AssetManager::UploadData tempData{};
                tinygltf::Model model = ast.getModel("../../../../models/dragon.glb");
                ast.getData(model,tempData);
                tempData.records[0].offsetVBO = stk.tailVBO;
                tempData.records[0].offsetIBO = stk.tailIBO;  //maybe it should be byteoffset??
                stk.tailVBO += tempData.records[0].totVertices;
                stk.tailIBO += tempData.records[0].totIndices;
                // for(auto r : tempData.records){
                //     modelStorage.storeModelRecord(r);
                // }
                
                std::cout << "size of vert, indices " << tempData.vertices.size() << ", " << tempData.indices.size() << "\n"; 
                stk.uploadVBOAndIBO(tempData.vertices,tempData.indices);
            }

            stk.render(); //pass gameobjs, render graph genned from inside
        stk.endFrame();
    }
    
    plt.shutdown();
}