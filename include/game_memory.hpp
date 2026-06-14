#pragma once
#include <variant>
struct GameVars{
    char name[16];
    std::variant<uint32_t, int , float, std::vector<uint32_t>> type{};
};

struct ScriptContext{
    std::vector<GameVars> vars{};

};

struct GameContext{
    std::vector<GameVars> globalVariables{};
    std::vector<ScriptContext> scripts{};
    
};

