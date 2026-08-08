#include "systems.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"           // For glm::transpose and core mat4 types
#include "sydney_physics.hpp"

namespace Sys{
    using Request = EngineAPI::Request;
    using AnimationRequest = EngineAPI::AnimationRequest;
    using Particle = Sydphys::Particle;
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
                                printf("entity #%d updating animation slot to '%lu' ...", req.EntityID, data.hash);
                            
                            }else{
                                printf("entity #%d creating animation slot and setting it to '%lu' ...", req.EntityID, data.hash);
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

        auto [transPool, rendPool, animPool] = reg.getPools<Transform, Renderable, Animated>();
        
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

                //if collider 
                //AssetRegistry::StaticModel& cube = ast.getStaticModelFromID(0);
                RenderPkt cubePkt{};
                cubePkt.type = Mesh::COLLIDER;
                glm::vec3 minV = glm::vec3{mdl.bounds.min.x, mdl.bounds.min.y, mdl.bounds.min.z} * 0.01f;
                glm::vec3 maxV = glm::vec3{mdl.bounds.max.x, mdl.bounds.max.y, mdl.bounds.max.z} * 0.01f;
                glm::vec3 center      = (minV + maxV) * 0.5f;
                glm::vec3 halfExtents = (maxV - minV) * 0.5f;

                // slim the box on X/Z to compensate for T-pose bind-pose arm spread
                float horizontalShrink = 0.35f; // tune this — 1.0 = no change, lower = slimmer
                halfExtents.x *= horizontalShrink;
                halfExtents.z *= horizontalShrink;

                glm::mat4 finalMat = glm::translate(glm::mat4(1.0f), center);
                                //* glm::scale(glm::mat4(1.0f), halfExtents);

                cubePkt.pc.modelSpace =  modelMat * finalMat ;
                
                packets.push_back(cubePkt);
                 
                


            }
            else{
                
                AssetRegistry::StaticModel& mdl = ast.getStaticModelFromID(rend.id);
                //printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesVBO/sizeof(Vertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
                glm::mat4 modelMat = transPool.get(e).matrix * mdl.normalizeMat;
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
   

    void finalizeTransforms(ECS::Registry& reg){
        auto& infoPool = reg.getPool<TransformInfo>();
        auto& transPool = reg.getPool<Transform>();
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
    
    void updatePhysics(ECS::Registry& reg, float dt)
    {
            /*you get a copy of vector filled with refs*/
            auto [transInfoPool, particPool] = reg.getPools<TransformInfo, Particle>();
            /*scuffed PHYSICS*/
            for (int i{}; i < particPool.count; i++){
                ECS::Entity e = particPool.dense[i];
                Particle& p = particPool.data[i];
                TransformInfo& tinfo = transInfoPool.get(e);
                integrateParticle(p,tinfo,dt);
                //p.integrate(,dt);                
            }
    }
}