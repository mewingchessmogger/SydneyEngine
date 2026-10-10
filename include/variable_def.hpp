#pragma once
//#include <variant>
//#include "string"
#include "glm/vec3.hpp" // inclue type_vec3 instead?

struct Variable{
    std::variant<glm::vec<3, float>,glm::quat, int, float, uint8_t, uint16_t, uint32_t, glm::vec<3, float>*,glm::quat*, uint8_t*,uint16_t*, int*, float*, uint32_t*> var{};
    std::string name{};
};


struct VariableAddress{
    std::variant<glm::vec<3, float>*,glm::quat*, int*,uint8_t*,uint16_t*, float*, uint32_t*> address{};
};

