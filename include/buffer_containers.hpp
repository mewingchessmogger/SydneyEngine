#pragma once
#include "field_defs.hpp"
#include "glm/mat4x4.hpp"



namespace PushC {
    struct Base {
        glm::mat4 model{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 proj{1.0f};
        // Chained Setters
        Base& setModel(const glm::mat4& m) { model = m; return *this; }
        Base& setView(const glm::mat4& v)  { view = v;  return *this; }
        Base& setProj(const glm::mat4& p)  { proj = p;  return *this; }
    };

    struct Model {
        glm::mat4 model{1.0f};
        uint32_t offsetVBO{};
        uint32_t offsetBoneBuffer{};
        glm::vec4 color{};
        //uint32_t secOffsetBoneBuffer{};
        Model& setModel(const glm::mat4& m) { model = m; return *this; }
        Model& setVertexBufferOffset(const uint32_t m) { offsetVBO = m; return *this;}
        Model& setBoneBufferOffset(const uint32_t m) { offsetBoneBuffer = m; return *this;}
        Model& setColor(const glm::vec4 clr) { color = clr; return *this;}
        
    };



}


namespace DynUBO {
    struct Base {
        FIELDS_UBO_BASE;
       
        
        Base& setIndx(uint64_t addr)            { indxAdress = addr; return *this; }
        Base& setVert(uint64_t addr)            { vertAdress = addr; return *this; }
        Base& setSkinVert(uint64_t addr)            { skinnedVertAdress = addr; return *this; }
        Base& setProjAddress(uint64_t addr)            { projectionAddress = addr; return *this; }    
        Base& setBoneAddress(uint64_t addr)            { animationAddress = addr; return *this; }    

    };

    struct CameraData{
        glm::mat4 view{};
        glm::mat4 proj{};
    };
    
    struct BoneMat{
        std::array<glm::mat4,256> matrices{};
    };

    struct ClrPick{
      uint32_t pickedID{};
      uint32_t padding[6];//cuz min size of ubo is 64 bytes on 3070 (mine)  
    }; //ABANDONED NEEDEDC VIEW AND PROJ SO WHATEVER

}
