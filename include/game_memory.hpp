#pragma once
#include <variant>
struct Variable{
    char name[16];
    std::variant<uint32_t, int , float, std::vector<uint32_t>> type{};
};

struct ScriptContext{
    std::vector<Variable> vars{};

};

struct GameContext{
    std::vector<Variable> globalVariables{};
    std::vector<ScriptContext> scripts{};
    
};

