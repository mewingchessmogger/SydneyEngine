#include "engine.hpp"
#include <iostream>
#include "platform_glfw.hpp"
#include "vulkan/vulkan.hpp"
#include "shaderc/shaderc.hpp"
#include "vk_stack.hpp"
#include "asset_manager.hpp"
#include "hash_model.hpp"
#include <fstream>
#include <string>
struct dummyScene {
    std::array<std::string, 3> Models;
    bool addedScnModels{};
    

};
void Engine::run(){
    
    PlatformGLFW plt{};
    AssetManager ast{};
    VulkanStack stk{};
    plt.initWindow(stk.WIDTH,stk.HEIGHT);
    stk.initInstance(plt);
    stk.initDevice(plt);
    stk.initSyncs();
    stk.initCommands();
    stk.initDescriptorStuff();
    stk.initBuffers();
    stk.initSwapchain();
    
    dummyScene scn = {{"foo.glb", "goo.glb", "asdf.glb"}, false};
    auto device = stk.ctx.device;
    auto cmdBuffers = stk.ctx.cmdBuffers;
    auto &allocator = stk.res.allocator;
    ModelStorage modelStorage{};


    std::ifstream f("src/shaders/test.frag");
    if (!f.is_open()) {
            std::cerr << "Error opening the file!";
            return;
    }
    std::string s;   
    while (std::getline(f,s)){
        std::cout << s << "\n";
    }
    f.close();
    while (plt.windowOpen()) {
        plt.pollEvents();
        //scn startframe

        if (scn.addedScnModels == true){
            scn.addedScnModels = false;
            AssetManager::UploadData tempData{};
            uint64_t bytesVertex{};
            uint64_t bytesIndex{}; 
            
           // IF MODEL ARLEADY EXIST SKIP SHIT TODO j11111111!!!!!!!!!!!!!!!!!!!!!!!
            // CHECK IF path is already hashed TODO !!!!!!!!!!!!!!!!!!!!!!

            for (int i{}; i < scn.Models.size(); i++){
                tinygltf::Model model = ast.getModel(scn.Models[i]);
                ast.getData(model,tempData);
                
                tempData.records[i].offsetVBO = stk.tailVBO;
                tempData.records[i].offsetIBO = stk.tailIBO;

                stk.tailVBO += tempData.records[i].totVertices;
                stk.tailIBO += tempData.records[i].totIndices;
                
            }
            for(auto r : tempData.records){
                modelStorage.storeModelRecord(r);
            }


            AllocatedBuffer stageVertex{};
            AllocatedBuffer stageIndex{};
            bytesVertex = tempData.vertices.size() * sizeof(Vertex);
            bytesIndex = tempData.indices.size() * sizeof(uint32_t);
            stk.res.createBuffer(BufferType::STAGING, bytesVertex, stageVertex);
            stk.res.createBuffer(BufferType::STAGING, bytesIndex, stageIndex);
            stk.res.uploadToBuffer(device,cmdBuffers[0], tempData.vertices.data(),bytesVertex,stageVertex, stk.res.vertexBuffer);
            stk.res.uploadToBuffer(device,cmdBuffers[0], tempData.indices.data(),bytesIndex,stageIndex, stk.res.indexBuffer);
            

            
            
        }

    }
    plt.shutdown();
}