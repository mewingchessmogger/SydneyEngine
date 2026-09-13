#include "systems.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"           // For glm::transpose and core mat4 types
#include "sydney_physics.hpp"
#include <glm/gtx/norm.hpp>
namespace Sys{
    using Request = EngineAPI::Request;
    using AnimationRequest = EngineAPI::AnimationRequest;
    using Particle = SydX::Particle;

    void processAPI(ECS::Registry& reg, AssetRegistry& ast, EngineAPI& api,  IAssetLoader& loader){
        
        for (auto& reqT: api.reqs) {

            switch (reqT.index()) { 
                case 0: // REQEUEST
                {// REQUEST{}
                    Request& req = std::get<Request>(reqT);
                    switch(req.cmd) 
                    {
                        case EngineAPI::LOAD_MODEL:{
                                printf("parsing %s ...", req.path.c_str());
                                if (ast.StringToIntegerSkinnedModelMap.find(req.path.c_str()) != ast.StringToIntegerSkinnedModelMap.end()){
                                    printf(" already uploaded '%s'\n", req.path.c_str());
                                    continue;
                                }
                                else if (ast.StringToIntegerStaticModelMap.find(req.path.c_str()) != ast.StringToIntegerStaticModelMap.end()){
                                    printf(" already uploaded '%s'\n", req.path.c_str());
                                    continue;
                                }
                               
                                loader.loadModel(req.path.c_str(), true);
                                //ldr.loadScene(req.path.c_str());
                                printf("done!\n");

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


                            if(animations.hasEntity(req.EntityID)){
                                printf("entity #%d updating animation slot to '%llu' ...", req.EntityID, data.hash);
                            
                            }else{
                                printf("entity #%d creating animation slot and setting it to '%llu' ...", req.EntityID, data.hash);
                            }

                            animations.assign(req.EntityID, comp); //updates OR creates inside
                            printf("done!\n");
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

        AssetRegistry::StaticModel& sphere = ast.getStaticModelFromString(std::string{"sphere.glb"});
        AssetRegistry::StaticModel& cube = ast.getStaticModelFromString(std::string{"cube.glb"});

        for (int i{}; i < collPool.count; i++){
            ECS::Entity e = collPool.dense[i];
            const Collider& coll = collPool.data[i];
            
            RawTransform& rawTrans = transPool.get(e);
            
            //printf("%f, %f, %f\n", rawTrans.getPosition().x, rawTrans.getPosition().y, rawTrans.getPosition().z);
            glm::mat4 broadMat = glm::translate(glm::mat4(1.0f),coll.offset + rawTrans.getPosition());
            broadMat = glm::scale(broadMat, glm::vec3(coll.broadRadius));
            
            RenderPkt broadPkt{.pc{.modelSpace = broadMat}, .type = Mesh::COLLIDER};
            
            broadPkt.pc.offsetVBO = sphere.baseOffsetBytesVBO / sizeof(Vertex);

            for(auto& mesh : sphere.meshes){
                broadPkt.offsetIBO = sphere.baseOffsetBytesIBO /sizeof(uint32_t); //  global , THIS AINT  a typo im too tired 
                broadPkt.indexCount = mesh.indexCount;
                broadPkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                
                //packets.push_back(broadPkt);
            }
            
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
    

    void integrateParticle(Particle& particle, TransformInfo& tinfo, float dt, bool accurateDamping = true){
        
        if (particle.inverseMass <= 0.0f) return;

        
        assert(dt >= 0);
        // p is position 
        // p' means derivative of p, p'' means second derivative and so on...
        tinfo.addPos(particle.vel * dt); // p = p + p'*t + [(1/2) * p''*t^2] <- acc part usually neglibigle in pos update so we ignore 
        //printf("Entity: %f, %f, %f", trans.worldPosition.x, trans.worldPosition.y,trans.worldPosition.z);
        particle.vel += particle.acc * dt; // p' = p' * d + p'' * t
        
        
        // CODE DOES THIS BUT THIS DOESNT FOLLOW MATH correctly! CHECK PAGE 53 IN IAN MILLINGTON
        if (accurateDamping){
            particle.vel *= powf(particle.damping,dt);    
        }else{
            particle.vel *= particle.damping;
        }
       
    }

/*  

    3x3 top left is LINEAR trans OF ROTATION * SCALING // 
mat4 = [a_x, b_x, c_x, T_x]     [x]
       [a_y, b_y, c_y, T_y] *   [y]
       [a_z, b_z, c_z, T_z]     [z]
       [0.0, 0.0, 0.0, 1.0]     [1.0]
*/


    SydX::GJK gjk{};
    SydX::EPA epa{};

    void updatePhysics(ECS::Registry& reg, float dt)
    {
            /*you get a copy of vector filled with refs*/
            auto [transInfoPool, rawTransPool, collPool, particPool] = reg.getPools<TransformInfo, RawTransform, Collider, SydX::Particle>();
            /*scuffed PHYSICS*/
            
            for (int i{}; i < particPool.count; i++){
                ECS::Entity e = particPool.dense[i];
                Particle& p = particPool.data[i];
                TransformInfo& tinfo = transInfoPool.get(e);
                integrateParticle(p,tinfo,dt);
            }

            for (int i{}; i < collPool.count; i++){
                ECS::Entity e1 = collPool.dense[i]; Collider& c1 = collPool.data[i];
                if(!rawTransPool.hasEntity(e1)){
                    return;
                }
                RawTransform& r1 = rawTransPool.get(e1);

                for (int j = i + 1; j < collPool.count; j++){
                    ECS::Entity e2 = collPool.dense[j]; Collider& c2 = collPool.data[j];
                    RawTransform& r2 = rawTransPool.get(e2);
                    
                    if (SydX::checkMidPhase(c1, c2, r1.matrix, r2.matrix)){
                        auto supportFn = [&](glm::vec3 dir) {return SydX::supportA_minus_B(dir, c1, c2, r1, r2);};

                        if(gjk.intersect(supportFn)){
                            printf("NARROW COLLISION!!!\n");
                            epa.run(gjk.abcd(), supportFn);
                            //epa.normal
                        }else{
                            printf("FALSE COLLISION!!!\n");
                        }
                        gjk.reset();


                        
                    }
                }
            }
            
        }
}