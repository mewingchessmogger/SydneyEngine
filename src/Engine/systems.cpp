#include "engine.hpp"
#include "asset_registry.hpp"
using Particle = physics::Particle;

void Engine::updatePhysics()
{
        /*you get a copy of vector filled with refs*/
        auto [transPool, particPool] = reg.getPools<Transform, Particle>();
        /*scuffed PHYSICS*/
        for (int i{}; i < particPool.count; i++){
            ECS::Entity e = particPool.dense[i];
            Particle& p = particPool.data[i];
            p.integrate(plt.deltaTime,true);
            transPool.get(e).position = p.pos;
            // std::cout << "VELOCITY of entity " << (int)e << ": " <<  p.vel.x << ", " <<  p.vel.y << ", " <<  p.vel.z << "\n";
        }
}

void Engine::prepareRenderables(Scene& scn, AssetRegistry& astReg){
    auto [transPool, rendPool] = reg.getPools<Transform, Renderable>();

    scn.packets.clear();

    /*FILL renderobjects array*/
    for (int i{}; i < rendPool.count; i++){
        ECS::Entity e = rendPool.dense[i];
        const Renderable& rend = rendPool.data[i];
        
        if(astReg.isSkinned(rend.id)){
            AssetRegistry::SkinnedModel& mdl = astReg.getSkinnedModelFromID(rend.id);
            //printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesSkinnedVBO/sizeof(SkinnedVertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
            glm::mat4 modelMat = transPool.get(e).matrix() * mdl.normalizeMat;
            Scene::RenderPkt pkt{};
            
            pkt.pc.modelSpace = modelMat;
            pkt.type = Scene::Mesh::SKINNED;
            pkt.pc.offsetVBO = mdl.baseOffsetBytesSkinnedVBO /sizeof(SkinnedVertex);

            for(auto& mesh : mdl.meshes){
                pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t); //  global
                pkt.indexCount = mesh.indexCount;
                pkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                scn.packets.push_back(pkt);
            }
        }
        else{
            
            AssetRegistry::StaticModel& mdl = astReg.getStaticModelFromID(rend.id);
            //printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesVBO/sizeof(Vertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
            glm::mat4 modelMat = transPool.get(e).matrix() * mdl.normalizeMat;
            Scene::RenderPkt pkt{};
            pkt.pc.modelSpace = modelMat;
            pkt.type = Scene::Mesh::STATIC;
            pkt.pc.offsetVBO = mdl.baseOffsetBytesVBO / sizeof(Vertex);

            for(auto& mesh : mdl.meshes){
                pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t); //  global
                pkt.indexCount = mesh.indexCount;
                pkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                scn.packets.push_back(pkt);
            }
        }


    }


}
