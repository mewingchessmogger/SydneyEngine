#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "asset_manager.hpp"
#include "hash_model.hpp"
glm::mat4 getNodeTransform(const tinygltf::Node& node) {
    if (node.matrix.size() == 16) {
        return glm::make_mat4(node.matrix.data());
    }
    glm::mat4 transform(1.0f);
    if (node.translation.size() == 3) {
        transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
    }
    if (node.rotation.size() == 4) {
        glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
        transform *= glm::mat4_cast(q);
    }
    if (node.scale.size() == 3) {
        transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
    }
    return transform;
}
void AssetManager::getData(tinygltf::Model& model, UploadData& uploadData) {
    
     
    ModelRecord record{};
    //we store in primitves local offsets, in future, model data might move around the gpu buffer therefore a global/local offset
    //system is optimal insated of hardcoding global offsets of gpu buffer..., the record will contain global offsets set outside this func
    uint32_t totalLocalVertices{}; 
    uint32_t totalLocalIndices{};

    for (size_t n = 0; n < model.nodes.size(); n++) {
        const auto& node = model.nodes[n];

        if (node.mesh < 0 || node.mesh >= model.meshes.size()) {
            continue;
        }

        glm::mat4 localMatrix = getNodeTransform(node);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(localMatrix)));

        Mesh meshData{};
        auto& mesh = model.meshes[node.mesh];
        
        for (auto& primitive : mesh.primitives) {
            primitiveData primData{};
            primData.indexOffset = totalLocalIndices;
            primData.vertexOffset= totalLocalVertices;
            
            // --- POSITION DATA ---
            const tinygltf::Accessor& posAccessor = model.accessors.at(primitive.attributes.at("POSITION"));
            const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
            const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];
            const float* posData = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

            // --- NORMAL DATA ---
            const float* normalData = nullptr;
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const tinygltf::Accessor& normalAccessor = model.accessors.at(primitive.attributes.at("NORMAL"));
                const tinygltf::BufferView& normalView = model.bufferViews[normalAccessor.bufferView];
                const tinygltf::Buffer& normalBuffer = model.buffers[normalView.buffer];
                normalData = reinterpret_cast<const float*>(&normalBuffer.data[normalView.byteOffset + normalAccessor.byteOffset]);
            }

            // --- TEXCOORD DATA ---
            const float* texCoordData = nullptr;
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& texCoordAccessor = model.accessors.at(primitive.attributes.at("TEXCOORD_0"));
                const tinygltf::BufferView& texCoordView = model.bufferViews[texCoordAccessor.bufferView];
                const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordView.buffer];
                texCoordData = reinterpret_cast<const float*>(&texCoordBuffer.data[texCoordView.byteOffset + texCoordAccessor.byteOffset]);
            }


            for (size_t i = 0; i < posAccessor.count; i++) {
                Vertex vertex{};
                glm::vec4 rawPos = glm::vec4(posData[3 * i + 0], posData[3 * i + 1], posData[3 * i + 2], 1.0f);
                vertex.pos = glm::vec3(localMatrix * rawPos);

                if (normalData) {
                    glm::vec3 rawNorm = glm::vec3(normalData[3 * i + 0], normalData[3 * i + 1], normalData[3 * i + 2]);
                    vertex.normal = glm::normalize(normalMatrix * rawNorm);
                } else {
                    vertex.normal = glm::vec3(0.0f);
                }
                vertex.texCoord = texCoordData ? glm::vec2(texCoordData[2 * i + 0], texCoordData[2 * i + 1]) : glm::vec2(0.0f);

                uploadData.vertices.push_back(vertex);
                totalLocalVertices++;
            }

            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
            const unsigned char* indexData = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];


            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                const uint16_t* indices16 = reinterpret_cast<const uint16_t*>(indexData);
                for (size_t i = 0; i < indexAccessor.count; i++){
                    uploadData.indices.push_back(indices16[i]); 
                    totalLocalIndices++;
                }  
            }
            else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                const uint32_t* indices32 = reinterpret_cast<const uint32_t*>(indexData);
                for (size_t i = 0; i < indexAccessor.count; i++){
                    uploadData.indices.push_back(indices32[i]);
                    totalLocalIndices++;
                } 
            }
            else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                const uint8_t* indices8 = reinterpret_cast<const uint8_t*>(indexData);
                for (size_t i = 0; i < indexAccessor.count; i++){
                    uploadData.indices.push_back(indices8[i]);
                    totalLocalIndices++;
                } 
            }

            
            primData.indexCount = indexAccessor.count;
            
            // // --- MATERIAL DATA ---
            // if (primitive.material >= 0) {
            //     auto& mat = model.materials[primitive.material];
            //     int texIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
            //     auto& clrFactor = mat.pbrMetallicRoughness.baseColorFactor;
            //     //data.baseColor = glm::vec4(clrFactor[0], clrFactor[1], clrFactor[2], clrFactor[3]);

            //     if (texIndex < 0) {
            //         primData.texIndex = -1;
            //     } else {
            //         primData.texIndex = model.textures[texIndex].source;
            //     }
            // }
            primData.texIndex = -1;

            meshData.primitives.push_back(primData);
        }

        record.meshes.push_back(meshData);
    }
    record.totVertices = totalLocalVertices;
    record.totIndices = totalLocalIndices;
    uploadData.records.push_back(record);
}