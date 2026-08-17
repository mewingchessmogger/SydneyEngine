#pragma once 
#include "variable_def.hpp"
//#include <vector>
#include "string_hasher.hpp"

#define REFLECT_1(a) \
    auto reflect() { \
        auto& [a] = *this; \
        return std::vector<Variable>{{a, #a}};\
    }\
    
#define REFLECT_2(a, b) \
    auto reflect(){ \
        auto& [a, b] = *this;\
        return std::vector<Variable>{{a, #a}, {b, #b}};\
    } \
    
#define REFLECT_3(a, b, c) \
    auto reflect() {\
        auto& [a, b, c] = *this;\
        return std::vector<Variable>{{a, #a}, {b, #b}, {c, #c}};\
    }\

#define REFLECT_4(a, b, c, d) \
    auto reflect() {\
        auto& [a, b, c, d] = *this;\
        return std::vector<Variable>{{a, #a}, {b, #b}, {c, #c}, {d, #d}};\
    }\



#define REFLECT_6(a, b, c, d, e ,f) \
    auto reflect() {\
        auto& [a, b, c, d, e, f] = *this;\
        return std::vector<Variable>{{a, #a}, {b, #b}, {c, #c}, {d, #d}, {e, #e}, {f, #f}};\
    }\



#define REFLECT_ADDRESS_1(a) \
    auto reflectAddress() { \
        auto& [a] = *this; \
        return std::vector<VariableAddress>{{&a}};\
    }\
    
#define REFLECT_ADDRESS_2(a, b) \
    auto reflectAddress() { \
        auto& [a, b] = *this;\
        return std::vector<VariableAddress>{{&a}, {&b}};\
    } \
    
#define REFLECT_ADDRESS_3(a, b, c) \
    auto reflectAddress() {\
        auto& [a, b, c] = *this;\
        return std::vector<VariableAddress>{{&a}, {&b}, {&c}};\
    }\

#define REFLECT_ADDRESS_4(a, b, c, d) \
    auto reflectAddress() {\
        auto& [a, b, c,d] = *this;\
        return std::vector<VariableAddress>{{&a}, {&b}, {&c}, {&d}};\
    }\

#define REFLECT_ADDRESS_6(a, b, c, d, e, f) \
    auto reflectAddress() {\
        auto& [a, b, c, d, e, f] = *this;\
        return std::vector<VariableAddress>{{&a}, {&b}, {&c}, {&d}, {&e}, {&f}};\
    }\


    //no need for pretty function or shitty typeid that invalidates on hot reloads due to dll data diseappring 
#define COMP_NAME(type) \
    static constexpr std::string_view getName(){\
        return #type; \
    }\
    static constexpr uint64_t getHash(){\
        return Hasher::stringview(#type);\
    }\
    