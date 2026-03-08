#version 450
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

layout(location = 0) in vec3 inNormal;
layout(location = 0) out vec4 outColor;

// layout(push_constant) uniform Constants {
//     mat4 view;
//     mat4 proj;
//     uint64_t indxAdress;
//     uint64_t vertAdress;
// } pc;

void main() {
    // Simple lighting using the normal passed from the Vert shader
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.0));

    vec3 absNormal = vec3(abs(inNormal.x),abs(inNormal.y),abs(inNormal.z));
    vec3 normal = inNormal * 2 - vec3(1.0);
    //float diff = max(dot(normalize(vec3()), lightDir), 0.20);
    
    outColor = vec4(absNormal, 1.0); 
}