#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <vector>
#include <typeinfo> // Required for typeid
#include <array>
#include "ecs_registry.hpp"
#include <sstream>
#include <cctype>

namespace Serde { //Seralizer//deserializer = Ser/de

    using vec3 = glm::vec<3,float>;
    
        
    struct ComponentPacket{
        std::string sName{}; // struct name from typeid
        std::vector<Variable> vars{};
        uint32_t id{};
    };
    std::string getNextToken(std::istream& is) {
        char ch{};
        std::string token{};
        
        // 1. Skip whitespace
        while (is.get(ch) && std::isspace(ch)); // while we can get a char and its a whitespace
        
        if (!is) return ""; // End of file
        
        // 2. Is it a symbol?
        if (ch == '{' || ch == '}') {
            token = ch;
            return token;
        }
        
        // 3. Otherwise, read until the next whitespace or symbol
        token += ch;
        while (is.peek() != EOF && !std::isspace(is.peek())) { // peek will check next character without changing anythign
            char next = is.peek();
            if (next == '{' || next == '}') 
            break;
            is.get(ch);
            token += ch;
        }
        return token;
    }
    
    void serialize(std::ostream& os, std::vector<Variable> &&vars){
        for( Variable& v : vars){
            if(std::holds_alternative<int>(v.var)){
                os << "      " << "int " << v.name << " ";
                int& ptr = std::get<int>(v.var);
                os <<  ptr << "\n";
            }
            else if(std::holds_alternative<float>(v.var)){
                os << "      " << "float " << v.name << " ";
                float& ptr = std::get<float>(v.var);
                os << ptr << "\n";
            }
            else if(std::holds_alternative<uint32_t>(v.var)){
                os << "      " << "uint32_t " << v.name << " ";
                uint32_t& ptr = std::get<uint32_t>(v.var);
                os << ptr << "\n";
            }
            else if(std::holds_alternative<vec3>(v.var)){
                os << "      " << "vec3 " << v.name << " ";
                vec3& ptr = std::get<vec3>(v.var);
                os << (ptr)[0] << " " << (ptr)[1] << " " << (ptr)[2]   << "\n";
            }
        }
    }

    template <typename T>
    void serializePool(std::ofstream& os, ECS::Registry& reg){
        auto& pool = reg.getPool<T>();
        std::type_index key = std::type_index(typeid(T));
        for(int e : pool.dense){
            os << "Entity " <<e <<   " {\n";

            os << "   " << key.name() << " {";
            std::vector<Variable> fields = pool.getComponentFields(e);
            if(!fields.empty()){
                os << "\n";
            }
            serialize(os, std::move(fields));
            if(!fields.empty()){
                os << "   ";
            }
            os << "}\n";
            os << "}\n";
        }

    }

    // void serializeReg(std::ofstream& os, ECS::Registry& reg){


    //     auto liveIDs = reg.getLiveIDs();

        
    //     for(auto e : liveIDs){
    //         os << "Entity " <<e <<   " {\n";
    //         auto& map = reg.getPoolMap();
    //         for(auto& [key, pool] : map){
    //             if(pool->hasEntity(e)){
    //                 os << "   " << key.name() << " {";
    //                 std::vector<Variable> fields = pool->getComponentFields(e);
    //                 if(!fields.empty()){
    //                     os << "\n";
    //                 }
    //                 serialize(os, std::move(fields));
    //                 if(!fields.empty()){
    //                     os << "   ";
    //                 }
    //                 os << "}\n";
    //             }

    //         }
    //         os << "}\n";
    //     }

    // }

    void serializePackets(std::ofstream& os, std::vector<ComponentPacket>& pkts){
        
        for(auto&pkt : pkts){
            os << "Entity " << pkt.id << " {\n";
            os << "   " << pkt.sName << " {\n";
            serialize(os, std::move(pkt.vars));
            os << "   }\n";
            os << "} \n";
        }
    }



    std::vector<ComponentPacket> deserializeFile(std::ifstream& is){
        enum class Scope{
            Entity, Struct, Count
        };

        std::string sName{};
        std::string type{};
        Variable currentVar{};
        uint32_t currentID{};
        std::vector<Variable> vars{};
        std::string token{};
        std::vector<Scope> scope{};
        std::vector<ComponentPacket> packets{};
        
        while(!is.eof()){
            token = getNextToken(is);
            if (token.empty()){
                break;
            }

            if(token == "Entity"){
                scope.push_back(Scope::Entity);
                currentID = (uint32_t)std::stoul(getNextToken(is));
            }
            else if(token == "struct"){
                scope.push_back(Scope::Struct);
                sName = token + " " +  getNextToken(is);
            }
            else if(token == "{" && scope.back() == Scope::Struct){
                
                if(is.peek() == '}'){
                    packets.push_back({sName, {}, currentID});
                    scope.pop_back();
                    continue;
                }

                while(is.peek() != '}' && is.peek() != EOF) {
                    type = getNextToken(is);
                    currentVar.name = getNextToken(is);
                    if(type == "float"){
                        currentVar.var = std::stof(getNextToken(is));
                    }
                    else if(type == "vec3"){
                        float f0 = stof(getNextToken(is));
                        float f1 = stof(getNextToken(is));
                        float f2 = stof(getNextToken(is));
                        currentVar.var = vec3{f0,f1,f2};
                    }
                    else if (type == "int"){
                        currentVar.var = std::stoi(getNextToken(is));

                    }
                    else if(type == "uint32_t"){
                        currentVar.var = (uint32_t)std::stoul(getNextToken(is));
                        
                    } else{
                        throw std::runtime_error(" TYPE DONT supported!!");
                    }
                    
                    std::cout << currentVar.name << "!!\n";
                    vars.push_back(currentVar);
                    while(std::isspace(is.peek()) && is.peek() != EOF){
                        is.get();
                    }
                }
                std::cout << "size of the vector is: " << vars.size() << "\n";
                packets.push_back({sName, std::move(vars), currentID});
                vars.clear();
                scope.pop_back();
            }
        }


        return packets;
    }

};
