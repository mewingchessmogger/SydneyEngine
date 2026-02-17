#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    
    
    bool operator==(const Vertex& other) const {
        return (pos == other.pos) && (normal == other.normal) && (texCoord == other.texCoord);
    }

};