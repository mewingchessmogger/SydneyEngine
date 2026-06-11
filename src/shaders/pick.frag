#version 450
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

layout(location = 0) out uvec4 outColor;

layout(push_constant) uniform Constants {
    mat4 model;
    uint offsetVBO;
    uint entityID;
} pc;

void main() {
    outColor = uvec4(pc.entityID,0,0,1); 
    
}


