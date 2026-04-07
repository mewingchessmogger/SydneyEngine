#pragma once
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
        Model& setModel(const glm::mat4& m) { model = m; return *this; }
    };



}


namespace DynUBO {
    struct Base {
        glm::mat4 model{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 proj{1.0f};
        glm::mat4 lightView{1.0f};
        glm::mat4 lightOrtho{1.0f};
        uint64_t indxAdress{0};
        uint64_t vertAdress{0};

        Base& setModel(const glm::mat4& m)       { model = m;  return *this; }
        Base& setView(const glm::mat4& v)       { view = v;  return *this; }
        Base& setProj(const glm::mat4& p)       { proj = p;  return *this; }
        Base& setLightView(const glm::mat4& lv) { lightView = lv; return *this; }
        Base& setLightOrtho(const glm::mat4& lo){ lightOrtho = lo; return *this; }
        Base& setIndx(uint64_t addr)            { indxAdress = addr; return *this; }
        Base& setVert(uint64_t addr)            { vertAdress = addr; return *this; }
    };
}