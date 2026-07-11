#version 450
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

layout(location = 0) in vec3 inNormal;
layout(location = 1) in flat ivec4 boneIDs;
layout(location = 2) in vec4 weights;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.0));

    vec3 absNormal = vec3(abs(inNormal.x),abs(inNormal.y),abs(inNormal.z));
    vec3 normal = inNormal * 2 - vec3(1.0);
    
    outColor = vec4(absNormal,1.0); 
}