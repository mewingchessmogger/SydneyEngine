#include "systems.hpp"
#include "sydney_physics.hpp"
#include "event_bus.hpp"

namespace Sys{
    using Request = EngineAPI::Request;
    using AnimationRequest = EngineAPI::AnimationRequest;
    using Particle = SydX::Particle;
    inline Variable JsonToVariable(const std::string& name, const nlohmann::json& jVal) {
        Variable var;
        var.name = name;

        if (jVal.is_number_integer()) {
            // Handles standard ints, counts, and uint8_t enums (like narrowShape)
            var.var = jVal.get<int>();
        } 
        else if (jVal.is_number_unsigned()) {
            // Handles explicit uint32_t fields
            var.var = jVal.get<uint32_t>();
        }
        else if (jVal.is_number_float()) {
            // Handles floats (radii, scale, damping, etc.)
            var.var = jVal.get<float>();
            
        } 
        
        else if (jVal.is_array() && jVal.size() == 3) {
            if(name != "rotation"){
                var.var = glm::vec3(
                    jVal[0].get<float>(), 
                    jVal[1].get<float>(), 
                    jVal[2].get<float>()
                );
            }else{
                var.var = glm::quat(glm::radians(glm::vec3(
                    jVal[0].get<float>(), 
                    jVal[1].get<float>(), 
                    jVal[2].get<float>()
                )));
            }
           
        }
        // else if (jVal.is_array() && jVal.size() == 4) {
        //     var.var = glm::quat(jVal[0].get<float>(), jVal[1].get<float>(), jVal[2].get<float>(), jVal[3].get<float>());
        // }
        else if (jVal.is_boolean()) {
            // Handles boolean flags
            var.var = jVal.get<bool>() ? 1 : 0;
        }

        return var;
    }
    void processAPI(ECS::Registry& reg, AssetRegistry& ast, EngineAPI& api,  IAssetLoader& loader){
        api.reqs.reserve(500);
        for (size_t i = 0; i < api.reqs.size(); ++i) {
            auto& reqT = api.reqs[i];
            
            switch (reqT.index()) { 
                case 0: // REQEUEST
                {// REQUEST{}
                    Request& req = std::get<Request>(reqT);
                    switch(req.cmd) 
                    {
                        case EngineAPI::LOAD_MODEL:{
                                printf("parsing %s ...", req.path.c_str());
                                if(!ast.isUploaded(req.path)){
                                    loader.loadModel(req.path.c_str(), true);
                                    printf("done!\n");
                                }else{
                                    printf("model alread uploaded...\n");
                                }
                                
                                break;
                        }			
                        case EngineAPI::ATTACH_MODEL:{
                                auto& renderables = reg.getPool<Renderable>();
                                uint32_t meshID = ast.getModelID(req.path);
                                
                                if(renderables.hasEntity(req.EntityID)){
                                    printf("entity #%d updating to '%s' (modelID #%d -> #%d)...", req.EntityID, req.path.c_str(), renderables.get(req.EntityID).id, meshID);
                                
                                }else{
                                    printf("entity #%d assigning  '%s', creating renderable component...", req.EntityID, req.path.c_str());
                                }
                                renderables.assign(req.EntityID, {meshID});
                                printf("done!\n");

                                break;
                        }
                        case EngineAPI::LOAD_SCENE:{
                                std::ifstream file(req.path,std::ios::binary);
                                nlohmann::ordered_json j = nlohmann::ordered_json::parse(file);
                                
                                if(j.contains("required_models") && j["required_models"].is_array()){
                                    for (const auto& model : j["required_models"]) {
                                    
                                        std::string path = model.get<std::string>();
                                        if(!ast.isUploaded(path)){
                                            loader.loadModel(path, true);
                                        }
                                    }
                                }
                                std::unordered_map<std::string, int> tagToEntity{};

                                if(j.contains("entities") && j["entities"].is_array()){
                                    for(const auto& entJson : j["entities"]){
                                        int id = (int)reg.createEntity();
                                        std::string tag = entJson.value("tag", ""); // the "" is a fallback value if cant find the key tag
                                        if(!tag.empty()){
                                            tagToEntity[tag] = id;
                                        }
                                        
                                        if (entJson.contains("model")) {
                                            api.attachModel(entJson["model"].get<std::string>(), id);
                                        }
                                        if (entJson.contains("animation")) {
                                            api.setAnimation(entJson["animation"].get<std::string>(), id);
                                        }
                                        if (entJson.value("is_game_camera", true)) {
                                            reg.emplace<Camera>(id);
                                            api.setGameCamera(id);
                                        }

                                        for (auto& [key, val] : entJson.items()) {
                                            // Skip non-component metadata keys
                                            if (key == "tag" || key == "model" || key == "animation" || 
                                                key == "is_game_camera" || key == "parent") {
                                                continue;
                                            }
                                            std::vector<Variable> vars{};
                                            if(val.is_object()){
                                                for(auto& [fieldName, fieldVal] :val.items()){
                                                    vars.push_back(JsonToVariable(fieldName, fieldVal));
                                                }
                                            }
                                            
                                            reg.deserializeComponent(id,key,std::move(vars));
                                        }

                                    }
                                    for (const auto& entJson : j["entities"]){
                                        if(entJson.contains("tag") && entJson.contains("parent")){
                                            std::string childTag = entJson["tag"];  
                                            std::string parentTag = entJson["parent"];
                                            
                                            int childId = tagToEntity[childTag];
                                            int parentId = tagToEntity[parentTag];
                                            printf(" child %d, parent : %d\n", childId, parentId);
                                            reg.setParent(childId, parentId);
                                        }
                                    }
                                        

                                }
                                
                                printf("done!\n");

                                break;
                                
                        }		


                    }	
                    break;
                }
                case 1:
                {
                    AnimationRequest& req = std::get<AnimationRequest>(reqT);
                    switch(req.cmd)
                    {
                        case EngineAPI::SET_ANIMATION:{
                            auto& animations = reg.getPool<Animated>();
                            auto& renderables = reg.getPool<Renderable>();
                            
                            if (!req.bypassLocked){
                                if(animations.hasEntity(req.EntityID)){    
                                    if (animations.get(req.EntityID).isLocked){
                                        break;
                                    }
                                }
                            }

                            

                            uint32_t meshID = renderables.get(req.EntityID).id;
                            AssetRegistry::SkinnedModel& mdl = ast.getSkinnedModelFromID(meshID);
                            


                            int animID = mdl.getAnimation(req.hash);
                            
                            AssetRegistry::AnimData& data = mdl.animationsData[animID];
                            
                            Animated comp = {.animationIndex = animID, .duration = data.duration, .offset = data.offsetInLocalBoneBuffer, .totalFrames = data.totalFrames, .isLocked = req.locked };


                            // if(animations.hasEntity(req.EntityID)){
                            //     printf("entity #%d updating animation slot to '%llu' ...", req.EntityID, data.hash);
                            
                            // }else{
                            //     printf("entity #%d creating animation slot and setting it to '%llu' ...", req.EntityID, data.hash);
                            // }

                            animations.assign(req.EntityID, comp); //updates OR creates inside
                            //printf("done!\n");
                            break;
                        }

                        break;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        api.reqs.clear();
    }
    


    void buildRenderPackets(ECS::Registry& reg, AssetRegistry& ast, std::vector<RenderPkt>& packets){
        
        packets.clear();

        auto [transPool, rendPool, animPool, collPool] = reg.getPools<RawTransform, Renderable, Animated, Collider>();
        
        for (int i{}; i < rendPool.count; i++){
            ECS::Entity e = rendPool.dense[i];
            const Renderable& rend = rendPool.data[i];
            
            if(ast.isSkinned(rend.id)){
                //printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesSkinnedVBO/sizeof(SkinnedVertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
                AssetRegistry::SkinnedModel& mdl = ast.getSkinnedModelFromID(rend.id);
                
                glm::mat4 modelMat = transPool.get(e).matrix;
                Animated& animated = animPool.get(e);
                
                
                int frameIndex = animated.getFrame();
                RenderPkt pkt{};
                pkt.type = Mesh::SKINNED;
                
                pkt.pc.modelSpace = modelMat;
                pkt.pc.offsetVBO = mdl.baseOffsetBytesSkinnedVBO /sizeof(SkinnedVertex);
                pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t);
                pkt.pc.offsetBoneBuffer = animated.offset + frameIndex * mdl.boneCount + mdl.baseOffsetBytesBoneBuffer / sizeof(glm::mat4); //HOLY reddit OPTIMIZATION 64 SAME AS 2^6 MEANING BIT SHIFT HOLY 

                
                for(auto& mesh : mdl.meshes){
                    pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t); //  global
                    pkt.indexCount = mesh.indexCount;
                    pkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                    packets.push_back(pkt);
                }

              
            }
            else{
                
                AssetRegistry::StaticModel& mdl = ast.getStaticModelFromID(rend.id);
                //printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesVBO/sizeof(Vertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
                glm::mat4 modelMat = transPool.get(e).matrix; //* mdl.normalizeMat;
                RenderPkt pkt{};
                pkt.pc.modelSpace = modelMat;
                pkt.type = Mesh::STATIC;
                pkt.pc.offsetVBO = mdl.baseOffsetBytesVBO / sizeof(Vertex);

                for(auto& mesh : mdl.meshes){
                    pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t); //  global , THIS AINT  a typo im too tired 
                    pkt.indexCount = mesh.indexCount;
                    pkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                    packets.push_back(pkt);
                }
            }


        }
    }
    
    void buildColliderPackets(ECS::Registry& reg, AssetRegistry& ast, std::vector<RenderPkt>& packets){
        
        auto [transPool, rendPool, collPool] = reg.getPools<RawTransform, Renderable, Collider>();
        AssetRegistry::StaticModel& sphere = ast.getStaticModelFromString(std::string{"sphere.glb"});
        AssetRegistry::StaticModel& cube = ast.getStaticModelFromString(std::string{"cube.glb"});

        for (int i{}; i < collPool.count; i++){
            ECS::Entity e = collPool.dense[i];
            const Collider& coll = collPool.data[i];
            
            RawTransform& rawTrans = transPool.get(e);
            
            //printf("%f, %f, %f\n", rawTrans.getPosition().x, rawTrans.getPosition().y, rawTrans.getPosition().z);
            glm::mat4 broadMat = glm::translate(glm::mat4(1.0f),coll.offset + rawTrans.getPosition());
            broadMat = glm::scale(broadMat, glm::vec3(coll.broadRadius));
            
            //RenderPkt broadPkt{.pc{.modelSpace = broadMat}, .type = Mesh::COLLIDER};
            
            // broadPkt.pc.offsetVBO = sphere.baseOffsetBytesVBO / sizeof(Vertex);
            // broadPkt.pc.color = {1.0f,1.0f,1.0f,1.0f};
            // for(auto& mesh : sphere.meshes){
            //     broadPkt.offsetIBO = sphere.baseOffsetBytesIBO /sizeof(uint32_t); //  global , THIS AINT  a typo im too tired 
            //     broadPkt.indexCount = mesh.indexCount;
            //     broadPkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                
            //     packets.push_back(broadPkt);
            // }
            
            RenderPkt narrowPkt{.type = Mesh::COLLIDER};
            narrowPkt.pc.color = {1.0f,0.0f, 0.0f, 1.0f};

            glm::mat4 narrowMat{};

            switch(coll.narrowShape){
                case Collider::SPHERE:{
                    narrowMat = glm::translate(glm::mat4(1.0f),coll.offset+ rawTrans.getPosition());
                    narrowMat = glm::scale(narrowMat, glm::vec3(coll.narrowRadius));
                    
                    break;
                }
                case Collider::AABB:{
                    narrowMat = glm::translate(glm::mat4(1.0f),coll.offset+ rawTrans.getPosition());
                    narrowMat = glm::scale(narrowMat, glm::vec3(coll.narrowExtents));
                   
                    break;
                }
                case Collider::OBB:{
                    narrowMat = glm::translate(glm::mat4(1.0f),coll.offset+ rawTrans.getPosition());
                    narrowMat = narrowMat * glm::mat4(rawTrans.getRotationMatrix());
                    
                    narrowMat = glm::scale(narrowMat, glm::vec3(coll.narrowExtents));                   
                    break;
                }
                default:
                    assert(0);
                    break;  
            }
            AssetRegistry::StaticModel& collMesh = (coll.narrowShape == Collider::SPHERE) ? sphere : cube;

            narrowPkt.pc.modelSpace = narrowMat; 
            narrowPkt.pc.offsetVBO = collMesh.baseOffsetBytesVBO / sizeof(Vertex);
            
            for(auto& mesh : collMesh.meshes){
                narrowPkt.offsetIBO = collMesh.baseOffsetBytesIBO /sizeof(uint32_t); //  global , THIS AINT  a typo im too tired 
                narrowPkt.indexCount = mesh.indexCount;
                narrowPkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local                   
                packets.push_back(narrowPkt);
            }


        }
    }
   

    void finalizeTransforms(ECS::Registry& reg){
        auto& infoPool = reg.getPool<TransformInfo>();
        auto& transPool = reg.getPool<RawTransform>();
        auto& hierPool = reg.getPool<ECS::Hierarchic>();
        
        if(reg.getIsHierarchyDirty()){
            throw std::runtime_error("'finalizeTransforms()', pools closed");
        }
        
        for (int i{}; i < infoPool.count; i++){
            ECS::Entity e = infoPool.dense[i];
            TransformInfo& info = infoPool.data[i];
                if(info.dirty){
                    if(!hierPool.hasEntity(e))// DO NOT UPDATE POSSIBLE ENTITYES IN HIERARCHIC, THEY NEED TO DERIVE FROM PARENT
                        transPool.assign(e,{info.getLocalMatrix()}); //updates or creates comp
                        //WHY NO DIRTY HERE? ITS CUZ WE SET THEM TO FALSE BELOW ALLR
                }
            
        }

        for (int i{}; i < hierPool.count; i++){
            ECS::Entity e = hierPool.dense[i];
            ECS::Hierarchic& hc = hierPool.data[i];

            TransformInfo& parentInfo = infoPool.get(hc.parent);
            TransformInfo& entInfo = infoPool.get(e);
            
            if(parentInfo.dirty || entInfo.dirty){// entity will move either way
                glm::mat4& parentMatrix = transPool.get(hc.parent).matrix;
                transPool.assign(e, {parentMatrix * entInfo.getLocalMatrix()});
                entInfo.dirty = true; //propagation of dirtyness ensuring children of this also moves                
            }

        }

        for (int i = 0; i < infoPool.count; i++) {
            infoPool.data[i].dirty = false;
        }
    }

    void updateAnimations(ECS::Registry& reg, float dt) {
        auto& animPool = reg.getPool<Animated>();
        for (int i{}; i < animPool.count; i++) {
            Animated& animated = animPool.data[i];
            float prevTime = animated.time;
            animated.time = fmod(animated.time + dt * animated.speed, animated.duration);
            if (animated.time < prevTime){
                animated.isLocked = false;
            }
        }
    }
    

     void collectBroadCandidates(ECS::Registry& reg, std::vector<SydX::CollisionCandidate>& candidates){
        auto [collPool, rigBodPool] = reg.getPools<Collider, SydX::RigidBody>();
        
        
        for (int i{}; i < rigBodPool.count; i++){
            ECS::Entity e1 = rigBodPool.dense[i]; 
            SydX::RigidBody& rb1 = rigBodPool.data[i];
            Collider& c1 = collPool.get(e1);

            for (int j = i + 1; j < rigBodPool.count; j++){
                ECS::Entity e2 = rigBodPool.dense[j];     
                SydX::RigidBody& rb2 = rigBodPool.data[j];
                Collider& c2 = collPool.get(e2);
                if (SydX::checkMidPhaseRB(c1, c2, rb1.position, rb2.position)){
                        candidates.push_back({{e1,e2}, {}});
                }
            }
        }

    } 
    

    void collectBroadRaycastCandidates(ECS::Registry& reg, std::vector<SydX::RaycastCandidates>& candidates){
        auto [collPool, raycastPool, rigBodPool] = reg.getPools<Collider, SydX::Raycast, SydX::RigidBody>();
        
        
        for (int i{}; i < raycastPool.count; i++){
            ECS::Entity e1 = raycastPool.dense[i]; 
            SydX::Raycast& ray = raycastPool.data[i];

            for (int j = 0; j < collPool.count; j++){
                ECS::Entity e2 = collPool.dense[j]; 
                Collider& collider = collPool.get(e2); 
                if(!rigBodPool.hasEntity(e2)){
                    continue;
                }
                glm::vec3& rbPos = rigBodPool.get(e2).position; 

                if (SydX::checkRaycastBroadPhase(ray, rbPos + collider.offset, collider.broadRadius)){
                    candidates.push_back({e1,e2});
                }
            }
        }

    } 

    void updatePhysics(ECS::Registry& reg, EventBus& bus, float dt)
    {
        static float lastDt{};

        

        auto [transInfoPool, collPool, rigBodPool, raycastPool] = reg.getPools<TransformInfo, Collider, SydX::RigidBody, SydX::Raycast>();// RAYCAT!!
        static SydX::EPA epa{};
        static SydX::GJK gjk{};
        
        static std::vector<SydX::CollisionCandidate> candidates{};
        static std::vector<SydX::RaycastCandidates> rayCandids{};
        
        glm::vec3 gravity = {0.0, -5.0, 0.0};
        
        candidates.clear();
        rayCandids.clear();
        collectBroadCandidates(reg, candidates);
        collectBroadRaycastCandidates(reg, rayCandids);
        int kSubsteps = 8;
        float substep = dt / kSubsteps;

        for(int step = 0; step < kSubsteps; step++){
            for (int i{}; i < rigBodPool.count; i++){
                SydX::RigidBody& rb = rigBodPool.data[i];
                if(rb.inverseMass <= 0.0){
                    continue;
                }
                rb.prevPosition = rb.position;
                rb.velocity = rb.velocity + substep * gravity;
                rb.position = rb.position + substep * rb.velocity;
            }
            
            for(auto& pair: candidates){

                auto [c1, c2] = collPool.get(pair.entities);
                auto [rb1, rb2] = rigBodPool.get(pair.entities);
            
                float w1 = rb1.inverseMass;
                float w2 = rb2.inverseMass;
                float wTotal = w1 + w2;
                if(wTotal <= 0.0){
                    continue;
                }
                //this supportfn will point RB1 - RB2 so arrow pointing towards RB1 from RB2
                auto supportFn = [&](glm::vec3 dir) {return SydX::supportA_minus_B_RigidBody(dir, c1, c2, rb1, rb2);};            
                SydX::GJK::ResultGJK res = gjk.intersect(supportFn);
                if(res.intersected){
                    SydX::EPA::ResultEPA res2 = epa.run(res.abcd, supportFn);
                    //printf("EPA Normal: (%.4f, %.4f, %.4f) | Depth: %.4f\n", res2.normal.x, res2.normal.y, res2.normal.z, res2.depth);

                    glm::vec3 lambda = (res2.normal * res2.depth) / wTotal;
                    rb1.position -= lambda * w1; // +-= depends on the support functions direction A -B or B-A
                    rb2.position += lambda * w2; 

                }
            }
            for (int i{}; i < rigBodPool.count; i++){
                SydX::RigidBody& rb = rigBodPool.data[i];
                if (rb.inverseMass <= 0.0f){
                    continue;
                }
                rb.velocity = (rb.position - rb.prevPosition)/substep;
                
            }
        }
    
        for (int i{}; i < rigBodPool.count; i++){
            SydX::RigidBody& rb = rigBodPool.data[i];
            ECS::Entity e = rigBodPool.dense[i];
            TransformInfo& tinfo = transInfoPool.get(e);
            tinfo.setPos(rb.position);
        }

        
        for(auto& pair: rayCandids){
            
                ECS::Entity rayID = pair.entities[0];
                SydX::Raycast& ray = raycastPool.get(rayID);
                SydX::RigidBody rayBody{.rotation = ray.direction, .position = ray.origin};
                
                Collider rayCollider{};
                rayCollider.narrowShape = Collider::OBB;
                rayCollider.narrowExtents = glm::vec3(0.001f, 0.001f, 100.0f);      



                ECS::Entity candidateColliderID = pair.entities[1];
                Collider& c1 = collPool.get(candidateColliderID);
                SydX::RigidBody& rb = rigBodPool.get(candidateColliderID);


                auto supportFn = [&](glm::vec3 dir) {return SydX::supportA_minus_B_General(dir, c1, rayCollider, rb, rayBody);};            
                SydX::GJK::ResultGJK res = gjk.intersect(supportFn);
                
                if(res.intersected){
                    bus.raycastHits.push_back({rayID, candidateColliderID});
                }
        }

        

    }
}

/*  

    3x3 top left is LINEAR trans OF ROTATION * SCALING // 
mat4 = [a_x, b_x, c_x, T_x]     [x]
       [a_y, b_y, c_y, T_y] *   [y]
       [a_z, b_z, c_z, T_z]     [z]
       [0.0, 0.0, 0.0, 1.0]     [1.0]
*/

    

    // void collectCollisions(ECS::Registry& reg, EventBus& bus, float dt, std::vector<SydX::CandidatePair>& pairs){
    //     static SydX::GJK gjk{};
    //     static SydX::EPA epa{};
    //     auto [rawTransPool, collPool, rigBodPool] = reg.getPools<RawTransform, Collider, SydX::RigidBody>();

    //     for (int i{}; i < collPool.count; i++){
    //             ECS::Entity e1 = collPool.dense[i]; Collider& c1 = collPool.data[i];
    //             if(!rawTransPool.hasEntity(e1)){
    //                 return;
    //             }
    //             RawTransform& r1 = rawTransPool.get(e1);

    //             for (int j = i + 1; j < collPool.count; j++){
    //                 ECS::Entity e2 = collPool.dense[j]; Collider& c2 = collPool.data[j];
    //                 RawTransform& r2 = rawTransPool.get(e2);
                    
    //                 if (SydX::checkMidPhase(c1, c2, r1.matrix, r2.matrix)){
    //                     auto supportFn = [&](glm::vec3 dir) {return SydX::supportA_minus_B(dir, c1, c2, r1, r2);};
    //                     if(gjk.intersect(supportFn)){
                            
    //                         SydX::ResultEPA result = epa.run(gjk.abcd(), supportFn);
                            
                            
    //                         if(!rigBodPool.hasEntity(e1) || !rigBodPool.hasEntity(e2)){
    //                             continue;
    //                         }
    //                         pairs.push_back({.A = e1, .B = e2, .n = result.normal, .depth = result.depth});
    //                         //add candidate
    //                     }else{
    //                         //printf("FALSE COLLISION!!!\n");
    //                     }
    //                     gjk.reset();


    //                 }

    //             }
    //         }
    // } 

/* for(Subscribed& sub : bus.subscribedEntities){
                int e = sub.ent;
                if(!collPool.hasEntity(e)){
                    continue;
                }
                ECS::Entity e1 = e;
                Collider& c1 = collPool.get(e);
                
                if(!rawTransPool.hasEntity(e1)){
                    return;
                }
                RawTransform& r1 = rawTransPool.get(e1);

                for (int j = 0; j < collPool.count; j++){
                    ECS::Entity e2 = collPool.dense[j];
                    if(e == e2){
                        continue;
                    }

                    Collider& c2 = collPool.data[j];
                    RawTransform& r2 = rawTransPool.get(e2);
                    
                    if (SydX::checkMidPhase(c1, c2, r1.matrix, r2.matrix)){
                        auto supportFn = [&](glm::vec3 dir) {return SydX::supportA_minus_B(dir, c1, c2, r1, r2);};
                        if(gjk.intersect(supportFn)){
                            bus.collisions.push_back({e,e2});
                        }
                        gjk.reset();
                    }

                }
            }
            */

            
        /*COLLISION RESOLUTION, NO ANGULAR KINEMATICS!*/
                
        // for (auto& pair : pairs){
            
        //     SydX::RigidBody& A = rigBodPool.get(pair.A);
        //     SydX::RigidBody& B = rigBodPool.get(pair.B);
        //     glm::vec3 n = pair.n;
        //     float depth = pair.depth;
        //     float totalInverseMass = A.inverseMass + B.inverseMass;
        //     if (totalInverseMass <= 0.0f) continue;

            
        //     float v_rel = glm::dot((B.vel - A.vel), n);
        //     float J{};
        //     if (v_rel < 0.0f){    
        //         J = -((A.bounciness + B.bounciness)/2.0f + 1.0f) * v_rel;
        //     }
        //     constexpr float beta = 0.3f;       
        //     constexpr float slop = 0.001f;      
        //     float baumgarteBias = ((beta / dt) * std::max(0.0f, depth - slop)) / 32.0f;
            
        //     float totalImpulse = J + baumgarteBias;
        //     if(totalImpulse > 0.0f){
        //         totalImpulse /= totalInverseMass; 
        //         A.vel = A.vel - (totalImpulse * A.inverseMass) * n;
        //         B.vel = B.vel + (totalImpulse * B.inverseMass) * n;       
        //     }
        // }
        
        //friction force
        // for (auto& pair : pairs) {
        //     Particle& A = rigBodPool.get(pair.A);
        //     Particle& B = rigBodPool.get(pair.B);
        //     glm::vec3 n = pair.n;

        //     glm::vec3 N = abs(n);
        //     int index = (N[0] > N[1]) ? ((N[0] > N[2]) ? 0 : 2) : ((N[1] > N[2]) ? 1 : 2);
        //     int t1 = (index + 1) % 3;
        //     int t2 = (index + 2) % 3;
            
        //     A.vel[t1] *= (1.0f - B.friction); 
        //     A.vel[t2] *= (1.0f - B.friction);
        //     B.vel[t1] *= (1.0f - A.friction); 
        //     B.vel[t2] *= (1.0f - A.friction);
        // }

        