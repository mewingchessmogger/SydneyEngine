#include "engine.hpp"
#include <iostream>
#include "platform_glfw.hpp"
#include "vulkan/vulkan.hpp"
#include "shaderc/shaderc.hpp"
#include "vk_stack.hpp"
#include "asset_manager.hpp"
#include "hash_model.hpp"

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

    dummyScene scn = {{"foo.glb", "goo.glb", "asdf.glb"}, false};
    auto device = stk.ctx.device;
    auto cmdBuffers = stk.ctx.cmdBuffers;
    auto &allocator = stk.mem.allocator;
    ModelStorage modelStorage{};
    
    
    while (plt.windowOpen()) {
        plt.pollEvents();


        if (scn.addedScnModels == true){
            scn.addedScnModels = false;
            AssetManager::ModelData md{};
            std::vector<tinygltf::Model> models{};
            uint64_t vertBytes{};
            uint64_t idxBytes{}; 
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            for (const auto& m: scn.Models){
                models.push_back(ast.getModel(m));
            }
            for (auto& m: models){
                for (const auto& mesh : m.meshes) {
                    for (const auto& primitive : mesh.primitives) {
                        // Get Position Accessor (usually the baseline for vertex count)
                        int posIdx = primitive.attributes.at("POSITION");
                        vertBytes += m.accessors[posIdx].count* sizeof(Vertex);

                        // Get Index Accessor
                        int indicesIdx = primitive.indices;
                        if (indicesIdx != -1) {
                            idxBytes += m.accessors[indicesIdx].count * sizeof(uint32_t);
                        }
                    }
                }
            }
            
            md.vertices.reserve(vertBytes/sizeof(Vertex));
            md.indices.reserve(idxBytes/sizeof(uint32_t));
            md.records.reserve(models.size());

            for (int i{}; i < models.size(); i++){
                auto& m = models[i];
                ast.getData(m,md);
                auto& rec = md.records[i];
                rec.offsetIBO = stk.tailVBO;
                rec.offsetVBO = stk.tailIBO;
                modelStorage.storeModel(rec);
                stk.tailVBO += md.
                
            }

            AllocatedBuffer vrtBuffer{};
            AllocatedBuffer idxBuffer{};
            stk.mem.createBuffer(BufferType::STAGING, vertBytes, vrtBuffer);
            stk.mem.createBuffer(BufferType::STAGING, idxBytes, idxBuffer);
            stk.mem.uploadToBuffer(device,cmdBuffers[0], md.vertices.data(),vertBytes,vrtBuffer, stk.mem.vertexBuffer);
            stk.mem.uploadToBuffer(device,cmdBuffers[0], md.indices.data(),idxBytes,idxBuffer, stk.mem.indexBuffer);
            
        }


        //start frame, get img, cmdbuffer if invalid swapchian, continue
        //evalgamescene -> 
            //check out scenes requested models, if model not processed before or isnt uploaded to gpu, processmodel, gen prims, eval textures, record upload to gpu 
            
            //check each objects dirtyflag
            //  if dirty 
            //  if change or first time, ensure upload of objects to idbo,ssbo
 
            //check if we have the wished shader effects, create pipelines accordingly, batch objects that use same pipeline,
            //process best way to render objects
            //instructions on how renderer should render objects.
            
        //imgui dis or ena here
        // start renderer with rendergraph and objs
        
        
        //iter objs, if want shadows, add to shadowpass,  
        
        //if wanna process adn upload
        //if ast.ismodeluploaded(scn.wishedModel)?
            //send models vertices, indicies , raw img data to vkstack, store their data in primtives storage
        // else 
        
        
        

        // AllocatedBuffer stagingBuffer{};
        // AllocatedImage img{};
        // AssetManager::imgInfo info = ast.processImageFile("foo");
        // uint32_t byteSize = info.texH * info.texW * 4;
        // uint32_t texH =  info.texH;
        // uint32_t texW =  info.texW;
        
        // stk.MemMgr.createBuffer(BufferType::SSBO,byteSize,stagingBuffer);
        // stk.MemMgr.requestImage(device,ImgType::TEXTURE,img,texW,texH);
        // stk.MemMgr.recordUploadTextureImage(device,cmdBuffers[0],img,info.ptr,texW,texH);
        // stbi_image_free(info.ptr); // safe here!!!!!!!!!!!!!!!
        // //need a SUBMIT BETWEEN HERE
        // vmaDestroyBuffer(allocator,stagingBuffer.buffer,stagingBuffer.alloc);

    }
    plt.shutdown();
}