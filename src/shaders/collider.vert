#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require



// Define the Buffer References (The "Types" for our pointers)

// Your Push Constant matches your C++ struct exactly
layout(push_constant) uniform Constants {
    mat4 model;
    uint offsetVBO;
    uint offsetBoneBuffer;
  //  uint secOffsetBoneBuffer;
} pc;

layout(buffer_reference, std140, buffer_reference_align = 16) readonly buffer CameraData{
    mat4 view;
    mat4 proj;
};

layout(set = 0, binding = 0) uniform UniformBufferObject {
    uint64_t indxAdress;
    uint64_t vertAdress;
    uint64_t skinnedVertAdress;
    uint64_t projectionAddress; 
    uint64_t animationAddress;
} ubo;



void main() {
    const vec3 collider[8] = vec3[8] ( 
    vec3(-1.0, -1.0, -1.0), // 0: Bottom-left-back
    vec3( 1.0, -1.0, -1.0), // 1: Bottom-right-back
    vec3( 1.0,  1.0, -1.0), // 2: Bottom-right-front
    vec3(-1.0,  1.0, -1.0), // 3: Bottom-left-front
    vec3(-1.0, -1.0,  1.0), // 4: Top-left-back
    vec3( 1.0, -1.0,  1.0), // 5: Top-right-back
    vec3( 1.0,  1.0,  1.0), // 6: Top-right-front
    vec3(-1.0,  1.0,  1.0)  // 7: Top-left-front
);

    const int LINE_INDICES[24] = int[24](
    // Bottom face loop
    0, 1,   1, 2,   2, 3,   3, 0,
    // Top face loop
    4, 5,   5, 6,   6, 7,   7, 4,
    // Vertical pillars
    0, 4,   1, 5,   2, 6,   3, 7
    );



    // Step 3: Project to Clip Space
    CameraData cam = CameraData(ubo.projectionAddress);
    
    gl_Position = cam.proj * cam.view * pc.model * vec4(collider[LINE_INDICES[gl_VertexIndex]], 1.0);
    
}