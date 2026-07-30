#include "systems.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"           // For glm::transpose and core mat4 types
#include "fysik_motor.hpp"

namespace Sys{
    using Request = EngineAPI::Request;
    using AnimationRequest = EngineAPI::AnimationRequest;
    using Particle = physics::Particle;
    void processAPI(ECS::Registry& reg, AssetRegistry& ast, EngineAPI& api,  IAssetLoader& loader){
        
        for (auto& reqT: api.reqs) {

            switch (reqT.index()) { 
                case 0: // REQEUEST
                {// REQUEST{}
                    Request& req = std::get<Request>(reqT);
                    switch(req.cmd) 
                    {
                        case EngineAPI::LOAD_MODEL:{
                                printf("parsing %s ...\n", req.path.c_str());
                                if (ast.StringToIntegerSkinnedModelMap.find(req.path.c_str()) != ast.StringToIntegerSkinnedModelMap.end()){
                                    printf(" already uploaded '%s'\n", req.path.c_str());
                                    continue;
                                }
                                else if (ast.StringToIntegerStaticModelMap.find(req.path.c_str()) != ast.StringToIntegerStaticModelMap.end()){
                                    printf(" already uploaded '%s'\n", req.path.c_str());
                                    continue;
                                }
                                
                                loader.loadModel(req.path.c_str());
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
                            
                            // comp.animationIndex = animID;
                            // comp.duration = data.duration;
                            // comp.totalFrames = data.totalFrames;
                            // comp.offset = data.offsetInLocalBoneBuffer; // now only frame * bonecount offset left
                            // comp.isLocked = req.locked;
                            

                            if(animations.hasEntity(req.EntityID)){
                                printf("entity #%d updating animation slot to '%s' ...", req.EntityID, data.name.c_str());
                            
                            }else{
                                printf("entity #%d creating animation slot and setting it to '%s' ...", req.EntityID, data.name.c_str());
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
                
                glm::mat4 modelMat = transPool.get(e).matrix() ;
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
                glm::mat4 modelMat = transPool.get(e).matrix() * mdl.normalizeMat;
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
 
    

    void propagateNodes(ECS::Registry& reg)
    {
            auto [transPool, nodePool] = reg.getPools<Transform, Node>();
            
            for (int i{}; i < nodePool.count; i++){
                ECS::Entity e = nodePool.dense[i];
                Node& node = nodePool.data[i];
                
                Transform& parent = transPool.get(e);
                Transform& child = transPool.get(node.child);

                glm::quat parentQuat = glm::quat(glm::radians(parent.worldRotation));
                glm::quat childLocalQuat = glm::quat(glm::radians(child.rotation)); // Local input!

                // Compute World Scale
                child.worldScale = parent.worldScale * child.scale;
                // Compute World Rotation
                glm::quat worldQuat = parentQuat * childLocalQuat;
                child.worldRotation = glm::degrees(glm::eulerAngles(worldQuat)); // World output!

                // Compute World Position
                glm::vec3 scaledOffset = parent.worldScale * child.position;
                child.worldPosition = parent.worldPosition + (parentQuat * scaledOffset); // World output!
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


    
    
    void updatePhysics(ECS::Registry& reg, float dt)
    {
            /*you get a copy of vector filled with refs*/
            auto [transPool, particPool] = reg.getPools<Transform, Particle>();
            /*scuffed PHYSICS*/
            for (int i{}; i < particPool.count; i++){
                ECS::Entity e = particPool.dense[i];
                Particle& p = particPool.data[i];
                p.integrate(dt,true);
                transPool.get(e).position = p.pos;
                // std::cout << "VELOCITY of entity " << (int)e << ": " <<  p.vel.x << ", " <<  p.vel.y << ", " <<  p.vel.z << "\n";
            }
    }
}