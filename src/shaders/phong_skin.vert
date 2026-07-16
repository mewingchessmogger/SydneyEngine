#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

// Define the structure of a single vertex
struct SkinnedVertex {
    vec3 pos;
    float pad0;
    vec3 normal;
    float pad1;
    vec2 uv;
    float pad2;
    float pad3;
    ivec4 boneIDs;
    vec4 weights;
};


// Define the Buffer References (The "Types" for our pointers)
layout(buffer_reference, scalar) readonly buffer IndexBuffer {
    uint indices[];
};

layout(buffer_reference, scalar) readonly buffer SkinnedVertexBuffer {
    SkinnedVertex vertices[];
};

layout(buffer_reference, std140, buffer_reference_align = 16) readonly buffer CameraData{
    mat4 view;
    mat4 proj;
};


// layout(buffer_reference, std140, buffer_reference_align = 16) readonly buffer BoneData{
//     mat4 matrices[256];
// };


// Your Push Constant matches your C++ struct exactly
layout(push_constant) uniform Constants {
    mat4 model;
    uint offsetVBO;
} pc;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    uint64_t indxAdress;
    uint64_t vertAdress;
    uint64_t skinnedVertexAdress;
    uint64_t projectionAddress;
    uint64_t animationAddress;
} ubo;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out flat ivec4 boneIDs;
layout(location = 2) out vec4 weights;
void main() {
    // Cast the raw 64-bit uints to our buffer types
    IndexBuffer  indexBuffer  = IndexBuffer(ubo.indxAdress);
    SkinnedVertexBuffer vertexBuffer = SkinnedVertexBuffer(ubo.skinnedVertexAdress);
    CameraData cam = CameraData(ubo.projectionAddress);
    //BoneData boneArr = BoneData(ubo.boneMatAddress);
    // Step 1: Fetch the index using the hardware counter
    uint vIndex = indexBuffer.indices[gl_VertexIndex] + pc.offsetVBO;

    // Step 2: Fetch the actual vertex data using that index
    SkinnedVertex v = vertexBuffer.vertices[vIndex];
    
    
    // mat4 boneTransform = boneArr.matrices[v.boneIDs[0]] * v.weights[0];
    // boneTransform     += boneArr.matrices[v.boneIDs[1]] * v.weights[1];
    // boneTransform     += boneArr.matrices[v.boneIDs[2]] * v.weights[2];
    // boneTransform     += boneArr.matrices[v.boneIDs[3]] * v.weights[3];
	
//* boneTransform
    gl_Position = cam.proj * cam.view   * vec4(v.pos, 1.0);
    boneIDs = v.boneIDs;
    weights = v.weights;

    mat3 normalMat = mat3(transpose(inverse(pc.model)));
    outNormal = normalize(normalMat * v.normal);

    
}