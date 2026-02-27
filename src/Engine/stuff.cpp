
        // if (scn.addedScnModels == true){
        //     scn.addedScnModels = false;
        //     AssetManager::UploadData tempData{};
            
            
        //    // IF MODEL ARLEADY EXIST SKIP SHIT TODO j11111111!!!!!!!!!!!!!!!!!!!!!!!
        //     // CHECK IF path is already hashed TODO !!!!!!!!!!!!!!!!!!!!!!

        //     for (int i{}; i < scn.Models.size(); i++){
        //         tinygltf::Model model = ast.getModel(scn.Models[i]);
        //         ast.getData(model,tempData);
                
        //         tempData.records[i].offsetVBO = stk.tailVBO;
        //         tempData.records[i].offsetIBO = stk.tailIBO;  //maybe it should be byteoffset??
            

        //         stk.tailVBO += tempData.records[i].totVertices;
        //         stk.tailIBO += tempData.records[i].totIndices;
                
        //     }
        //     for(auto r : tempData.records){
        //         modelStorage.storeModelRecord(r);
        //     }

            
        //     AllocatedBuffer stageVertex{};
        //     AllocatedBuffer stageIndex{};
        //     uint64_t bytesVertex = tempData.vertices.size() * sizeof(Vertex);
        //     uint64_t bytesIndex = tempData.indices.size() * sizeof(uint32_t);
        //     stk.res.createBuffer(BufferType::STAGING, bytesVertex, stageVertex);
        //     stk.res.createBuffer(BufferType::STAGING, bytesIndex, stageIndex);
        //     stk.res.uploadToBuffer(device,cmdBuffers[0], tempData.vertices.data(),bytesVertex,stageVertex, stk.res.vertexBuffer);
        //     stk.res.uploadToBuffer(device,cmdBuffers[0], tempData.indices.data(),bytesIndex,stageIndex, stk.res.indexBuffer);
            
            
            
            
        // }
    //     auto device = stk.ctx.device;
    // auto cmdBuffers = stk.ctx.cmdBuffers;
    // auto &allocator = stk.res.allocator;
   



// struct dummyScene {
//     std::array<std::string, 3> Models;
//     bool addedScnModels{};
    

// };