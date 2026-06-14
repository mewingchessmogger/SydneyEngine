#pragma once
#include <variant>
#include "string"
#include "glm/vec3.hpp"
struct Variable{
    std::variant<glm::vec3*, int*, float*, uint32_t*> ptr{};
    std::string_view name{};
};
