#version 450
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

// Define the structure of a single vertex
struct Vertex {
    vec3 pos;
    float pad0;
    vec3 normal;
    float pad1;
    vec2 uv;
    float pad2;
    float pad3;
};


// Define the Buffer References (The "Types" for our pointers)
layout(buffer_reference, scalar) readonly buffer IndexBuffer {
    uint indices[];
};

layout(buffer_reference, scalar) readonly buffer VertexBuffer {
    Vertex vertices[];
};

// Your Push Constant matches your C++ struct exactly
layout(push_constant) uniform Constants {
    mat4 model;
    uint offsetVBO;
} pc;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightView;
    mat4 lightOrtho;
    uint64_t indxAdress;
    uint64_t vertAdress;
} ubo;

layout(location = 0) out vec3 outNormal;

void main() {
    // Cast the raw 64-bit uints to our buffer types
    IndexBuffer  indexBuffer  = IndexBuffer(ubo.indxAdress);
    VertexBuffer vertexBuffer = VertexBuffer(ubo.vertAdress);

    // Step 1: Fetch the index using the hardware counter
    uint vIndex = indexBuffer.indices[gl_VertexIndex] + pc.offsetVBO;

    // Step 2: Fetch the actual vertex data using that index
    Vertex v = vertexBuffer.vertices[vIndex];

    // Step 3: Project to Clip Space
    gl_Position = ubo.proj * ubo.view * pc.model * vec4(v.pos, 1.0);

    
    mat3 normalMat = mat3(transpose(inverse(ubo.model)));
    outNormal = normalize(normalMat * v.normal);

    
}