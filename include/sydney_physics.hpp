#pragma once 

#include "glm/matrix.hpp"      
#include <glm/gtx/norm.hpp>
#include "reflections.hpp"

#include "engine_components.hpp"
namespace SydX {
    using real = float;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using  glm::dot;
    using  glm::cross;
    using  glm::normalize;
    using glm::transpose;
    using  vec3 = glm::vec3;
    using  vec4 = glm::vec4;
    using mat3 = glm::mat3;
   
    struct Particle{
        real inverseMass{};
        vec3 vel{};
        vec3 acc{};
        real damping{};
        COMP_NAME(Particle);
    };

    bool checkMidPhase(Collider& c1, Collider& c2, glm::mat4& r1, glm::mat4& r2);
    
    glm::vec3 support(glm::vec3 dir, Collider& c1, RawTransform& r1);
    glm::vec3 supportA_minus_B(glm::vec3 dir, Collider& c1, Collider& c2, RawTransform& r1, RawTransform& r2);

    struct GJK{
        
        vec3 dir;
        vec3 a, b, c, d;
        uint32_t n{}; // n refers to the amount of stored points excluding a!
    
        bool update(){
           
            switch(n){

                case 0: {// AAAH CASE
                    b = a;
                    dir = -a;
                    n = 1;
                    return false;
                }
                case 1:{// LINE CASE
                    vec3 ab = b-a;
                    vec3 ao = -a;
                    
                    if (dot(ab, ao) > 0){
                        c = b;
                        b = a;
                        dir = cross(cross(ab, ao), ab);
                        n = 2;
                    }else{
                        n = 0;
                        dir = ao;
                    }

                    return false;
                }
                case 2: {// TRIANGLE CASE 
                    vec3 ab = b-a;
                    vec3 ac = c-a;
                    vec3 ao = -a;
                    vec3 norm_abc = cross(ab,ac);
                    
                    if (dot(cross(norm_abc, ac), ao) > 0){

                        if(dot(ac,ao) > 0){
                            b = c;
                            dir = cross(cross(ac,ao),ac);
                            n = 1;
                            return false;
                        }
                        else{
                            n = 1;
                            return update();
                        }
                        
                    }
                    else{
                        if(dot(cross(ab, norm_abc), ao) > 0){
                            n = 1;
                            return update();
                        }
                        else{
                            //this is where we have concluded origo is bound inside a infinte triangle strip of our points, imagine a triangular infinte pillar going through the triangles in the direciton of its normal, we have determined that origio is inside it, 
                            n = 3;
                            
                            d = c;
                            c = b;
                            b = a;

                            /*convince yourself that using this triangle which normal pointing towards origo, when interacted with hte future point a, with all 3 face have normals faceing away from origo*/
                            if(dot(norm_abc, ao) > 0){ 
                                dir = norm_abc;
                            }else{
                                std::swap(c, d);
                                dir = -norm_abc;
                            }
                        }
                        return false;
                    }
                }
                case 3:{ // TETRAHEDRON CASE
                    vec3 ab = b-a;
                    vec3 ac = c-a;
                    vec3 ad = d-a;
                    
                    vec3 ao =  -a;
                    vec3 abc = cross(ab,ac);
                    vec3 acd = cross(ac,ad);
                    vec3 adb = cross(ad,ab);

                    if (dot(abc,ao) > 0){
                        n = 2; 
                        return update();
                    }
                    if (dot(acd,ao) > 0){
                        n = 2;
                        b = c;
                        c = d;
                        return update();
                    }
                    if (dot(adb,ao) > 0){
                        n = 2;
                        c = b;
                        b = d;
                        //printf("adb\n");
                        return update();
                    }

                    return true;
                }
                default:{
                    assert(0);
                    break;
                }
            }

            
        }

        
        template <typename supportFunc>
        bool intersect(supportFunc&& supportFn){
            
            dir = vec3{1.0, 0.0, 0.0}; 
           
            for (int i {}; i < 32; i++){
                a = supportFn(dir);
            
                if(dot(a, dir) < 0.0){
                    //printf("%d", i);
                    return false;
                }
                
                if (update()){
                    //printf("%d", i);
                    return true;
                }
            }
            return false;
        }
        std::array<vec3,4> abcd(){
            return {a,b,c,d};
        }
        void reset(){
            a = {};
            b = {};
            c = {};
            d = {};
            n = {};
        }
    };
    struct EPA{
        vec3 contactNormal{};
        float penDepth{};
        vec3 a, b, c, d;
        std::vector<vec3> vertexList{};
        std::vector<std::array<vec3,3>> triangleList{};
        std::vector<vec3> edgeList{};

        template <typename supportFunc>
        void run(std::array<vec3,4> points, supportFunc&& supportFn){
           glm::vec3 a = points[0], b = points[1], c = points[2], d = points[3];
            // Compute outward-facing normals via cross products
            glm::vec3 n_abc = glm::cross(b - a, c - a);
            glm::vec3 n_bdc = glm::cross(d - b, c - b);
            glm::vec3 n_acd = glm::cross(c - a, d - a);
            glm::vec3 n_adb = glm::cross(d - a, b - a);

            // If dot(normal, vertex) > 0, the face normal points outward (away from origo)
            if (glm::dot(n_abc, -a) > 0.0f) printf("abc outward: %.3f\n", glm::dot(n_abc, -a));
            if (glm::dot(n_bdc, -b) > 0.0f) printf("bdc outward: %.3f\n", glm::dot(n_bdc, -b));
            if (glm::dot(n_acd, -a) > 0.0f) printf("acd outward: %.3f\n", glm::dot(n_acd, -a));
            if (glm::dot(n_adb, -a) > 0.0f) printf("adb outward: %.3f\n", glm::dot(n_adb, -a));
                        
            /*
            while 1:
                pick closest face
                    if no closer than previous return 
                    else:
                    

                
            
            
            */


        }
    };
  
};