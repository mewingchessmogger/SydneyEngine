#pragma once 
#include "variable_def.hpp"
#include <vector>


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



#define REFLECT_ADDRESS_1(a) \
    auto reflectAddress() { \
        auto& [a] = *this; \
        return std::vector<VariableAddress>{{&a,}};\
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

