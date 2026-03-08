#pragma once
#include "glm/mat4x4.hpp"

namespace PushC {
    
    struct Base {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        uint64_t indxAdress{};
        uint64_t vertAdress{};
    };

    
}


namespace DynUBO { 

    struct Base { 
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::mat4 lightView;
        glm::mat4 lightOrtho;
        uint64_t indxAdress{};
        uint64_t vertAdress{};
    };
};