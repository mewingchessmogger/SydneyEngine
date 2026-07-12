#include "engine.hpp"
#include "asset_registry.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/matrix.hpp"           // For glm::transpose and core mat4 types
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

const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string nodeName)
	{
		for (uint32_t i = 0; i < animation->mNumChannels; i++)
		{
			const aiNodeAnim* nodeAnim = animation->mChannels[i];
			if (std::string(nodeAnim->mNodeName.data) == nodeName)
			{
				return nodeAnim;
			}
		}
		return nullptr;
	}

	// Returns a 4x4 matrix with interpolated translation between current and next frame
	aiMatrix4x4 interpolateTranslation(float time, const aiNodeAnim* pNodeAnim)
	{
		aiVector3D translation;

		if (pNodeAnim->mNumPositionKeys == 1)
		{
			translation = pNodeAnim->mPositionKeys[0].mValue;
		}
		else
		{
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
			{
				if (time < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
				{
					frameIndex = i;
					break;
				}
			}

			aiVectorKey currentFrame = pNodeAnim->mPositionKeys[frameIndex];
			aiVectorKey nextFrame = pNodeAnim->mPositionKeys[(frameIndex + 1) % pNodeAnim->mNumPositionKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiVector3D& start = currentFrame.mValue;
			const aiVector3D& end = nextFrame.mValue;

			translation = (start + delta * (end - start));
		}

		aiMatrix4x4 mat;
		aiMatrix4x4::Translation(translation, mat);
		return mat;
	}

	// Returns a 4x4 matrix with interpolated rotation between current and next frame
	aiMatrix4x4 interpolateRotation(float time, const aiNodeAnim* pNodeAnim)
	{
		aiQuaternion rotation;

		if (pNodeAnim->mNumRotationKeys == 1)
		{
			rotation = pNodeAnim->mRotationKeys[0].mValue;
		}
		else
		{
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
			{
				if (time < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
				{
					frameIndex = i;
					break;
				}
			}

			aiQuatKey currentFrame = pNodeAnim->mRotationKeys[frameIndex];
			aiQuatKey nextFrame = pNodeAnim->mRotationKeys[(frameIndex + 1) % pNodeAnim->mNumRotationKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiQuaternion& start = currentFrame.mValue;
			const aiQuaternion& end = nextFrame.mValue;

			aiQuaternion::Interpolate(rotation, start, end, delta);
			rotation.Normalize();
		}

		aiMatrix4x4 mat(rotation.GetMatrix());
		return mat;
	}


	// Returns a 4x4 matrix with interpolated scaling between current and next frame
	aiMatrix4x4 interpolateScale(float time, const aiNodeAnim* pNodeAnim)
	{
		aiVector3D scale;
		//check num of keys if only one then 0
		if (pNodeAnim->mNumScalingKeys == 1)
		{
			scale = pNodeAnim->mScalingKeys[0].mValue;
		}

		//oooh more interesting herfe we actually check our time against the nodes scalin keys.mtime until its less than time and process that key, 
		else
		{
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
			{
				if (time < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
				{
					frameIndex = i;
					break;
				}
			}
			//oooh we got scaling key for this keys and next keys
			aiVectorKey currentFrame = pNodeAnim->mScalingKeys[frameIndex];
			aiVectorKey nextFrame = pNodeAnim->mScalingKeys[(frameIndex + 1) % pNodeAnim->mNumScalingKeys]; // aah so if we are at last frame then we interpolate wiht start frame

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiVector3D& start = currentFrame.mValue;
			const aiVector3D& end = nextFrame.mValue;

			scale = (start + delta * (end - start));
		}

		aiMatrix4x4 mat;
		aiMatrix4x4::Scaling(scale, mat);
		return mat;
	}

void Engine::readNodeHierarchy(float animationTime,aiAnimation* pAnimation, const aiNode* pNode, const aiMatrix4x4& parentTransform, const aiMatrix4x4& globalInverseTransform, std::vector<RenderPkt>& packets, RenderPkt templatePkt, AssetRegistry::SkinnedModel& mdl){
    
    for(int i{}; i < pNode->mNumMeshes;i++){
        uint32_t m = pNode->mMeshes[i];
        AssetRegistry::SkinnedMeshData& mesh = mdl.meshes[m];
        RenderPkt pkt = templatePkt;
        pkt.indexCount = mesh.indexCount;
        pkt.offsetIBO += mesh.baseIndexLocalIBO;
        
        packets.push_back(pkt);
    }

    std::string nodeName = std::string(pNode->mName.C_Str());

    aiMatrix4x4 NodeTransformation(pNode->mTransformation);
    const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, nodeName);

    if (pNodeAnim)
	{
			// Get interpolated matrices between current and next frame
			aiMatrix4x4 matScale = interpolateScale(animationTime, pNodeAnim);
			aiMatrix4x4 matRotation = interpolateRotation(animationTime, pNodeAnim);
			aiMatrix4x4 matTranslation = interpolateTranslation(animationTime, pNodeAnim);
			NodeTransformation = matTranslation * matRotation * matScale;
	}

    aiMatrix4x4 GlobalTransformation = parentTransform * NodeTransformation;

    if(mdl.boneNameToIndexMap.find(nodeName) != mdl.boneNameToIndexMap.end()){
        int boneID = mdl.boneNameToIndexMap[nodeName];
        aiMatrix4x4 finalMat = globalInverseTransform * GlobalTransformation * mdl.boneMats[boneID];

        mdl.finalBoneMatrices[boneID] = glm::transpose(glm::make_mat4(&finalMat.a1));
    }
    
    for(int i{}; i < pNode->mNumChildren;i++){
        readNodeHierarchy(animationTime, pAnimation, pNode->mChildren[i], GlobalTransformation, globalInverseTransform, packets, templatePkt, mdl);
    }
    

}
void Engine::parseSceneNodes(const aiScene* scn, std::vector<RenderPkt>& packets, RenderPkt templatePkt, AssetRegistry::SkinnedModel& mdl){
    float TicksPerSecond = (float)(scn->mAnimations[0]->mTicksPerSecond != 0 ? scn->mAnimations[0]->mTicksPerSecond : 25.0f);
    static float TimeInTicks;
	TimeInTicks += plt.deltaTime * TicksPerSecond;
    float animTime = fmod(TimeInTicks, (float)scn->mAnimations[0]->mDuration);
    aiMatrix4x4 globalInverseTransform = scn->mRootNode->mTransformation.Inverse();
    aiMatrix4x4 identity = {};
    readNodeHierarchy(animTime, scn->mAnimations[0], scn->mRootNode, identity, globalInverseTransform, packets, templatePkt, mdl);

}


void Engine::prepareRenderables(std::vector<RenderPkt>& packets){
    auto [transPool, rendPool] = reg.getPools<Transform, Renderable>();

    packets.clear();
    AssetRegistry& astReg = ldr.getAssetReg();
    /*FILL renderobjects array*/
    for (int i{}; i < rendPool.count; i++){
        ECS::Entity e = rendPool.dense[i];
        const Renderable& rend = rendPool.data[i];
        
        if(astReg.isSkinned(rend.id)){
            AssetRegistry::SkinnedModel& mdl = astReg.getSkinnedModelFromID(rend.id);
            //printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesSkinnedVBO/sizeof(SkinnedVertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
            glm::mat4 modelMat = transPool.get(e).matrix() * mdl.normalizeMat;
            RenderPkt pkt{};
            
            pkt.pc.modelSpace = modelMat;
            pkt.type = Mesh::SKINNED;
            pkt.pc.offsetVBO = mdl.baseOffsetBytesSkinnedVBO /sizeof(SkinnedVertex);
            pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t);
            parseSceneNodes(ldr.scenes[0], packets, pkt, mdl);
            
            // for(auto& mesh : mdl.meshes){
            //     pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t); //  global
            //     pkt.indexCount = mesh.indexCount;
            //     pkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
            //     packets.push_back(pkt);
            // }
        }
        else{
            
            AssetRegistry::StaticModel& mdl = astReg.getStaticModelFromID(rend.id);
            //printf("ID: %d, name: %s, offsetVBO is: %d, globalOffsetIBO is %d\n",rend.id, mdl.name.c_str(),mdl.baseOffsetBytesVBO/sizeof(Vertex),mdl.baseOffsetBytesIBO / sizeof(uint32_t));
            glm::mat4 modelMat = transPool.get(e).matrix() * mdl.normalizeMat;
            RenderPkt pkt{};
            pkt.pc.modelSpace = modelMat;
            pkt.type = Mesh::STATIC;
            pkt.pc.offsetVBO = mdl.baseOffsetBytesVBO / sizeof(Vertex);

            for(auto& mesh : mdl.meshes){
                pkt.offsetIBO = mdl.baseOffsetBytesIBO /sizeof(uint32_t); //  global
                pkt.indexCount = mesh.indexCount;
                pkt.offsetIBO += mesh.baseIndexLocalIBO;     //global +local           
                packets.push_back(pkt);
            }
        }


    }


}
