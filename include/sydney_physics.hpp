#pragma once 

#include "glm/matrix.hpp"      
#include <glm/gtx/norm.hpp>
#include "reflections.hpp"

#include "engine_components.hpp"
#include <limits>
#include <utility>
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
        float bounciness = 0.5; // 1.0f : completely elastic, 0.0f : Everything becomes heat
        COMP_NAME(Particle);
    };

    struct CandidatePair{
        int A{};
        int B{};
        vec3 n{};
        float depth{};
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
                            //imagine a triangular infinte pillar going through the triangles in the direciton of its normal, we have determined that origio is inside the pillar, 
                            n = 3;
                            
                            d = c;
                            c = b;
                            b = a;

                            /*convince yourself that using this triangle which normal WILL point towards origo,
                             with that choice, the future three faces will ALWAYS have a normal that points OUTWARDS from ORIGO IF we did enclose it properly,*/
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
        std::vector<std::array<vec3,3>> triangleList{};
        //std::vector<std::array<vec3,2> edgeList{};
        std::vector<vec3> vertexList{};
       

        template <typename supportFunc>
        void run(std::array<vec3,4> points, supportFunc&& supportFn){
            double lastFaceDistance = std::numeric_limits<double>::infinity();
            glm::vec3 a = points[0], b = points[1], c = points[2], d = points[3];
            
            triangleList = {{a,b,c}, {b,d,c}, {a,c,d}, {a,d,b}};
            auto result = getClosestFace();
        }


         std::pair<vec3, double> getClosestFace(){
            int index{};
            double smallestDistance = std::numeric_limits<double>::max();
            
            double distance{};
            vec3 currentNormal{};
            for(auto& tri : triangleList){
                vec3 normalFace = normalize(cross(tri[1]-tri[0], tri[2]-tri[0])); 
                distance = abs(dot(normalFace, tri[0]));
                if (distance < smallestDistance){
                    smallestDistance = distance;
                    currentNormal = normalFace;
                }
            }
            return std::pair<vec3, double>(currentNormal, distance);
        }
    };
  
};

/*   From https://allenchou.net/2013/12/game-physics-contact-generation-epa/
1.Take over the simplex from GJK when GJK terminated, and “blow up” the simplex to a tetrahedron if it contains less than 4 vertices.
2. Use the 4 faces (triangles) of the tetrahedron to construct an initial polytope.
3. Pick the closest face of the polytope to the origin.
4. If the closest face is no closer (by a certain threshold) to the origin than the previously picked one, go to 8.
5. Remove the closest face, use the face normal (outward pointing) as the search direction to find a support point on the CSO.
6. Remove all faces from the polytope that can be “seen” by this new support point,
    and add new faces to cover up the “hole” on the polytope,
    where all new faces share the new support point as a common vertex (this is the expanding part of the algorithm).
7. Go to 3.
8..Project the origin onto the closest triangle. This is our closest point to the origin on the CSO’s boundary. 
    Compute the barycentric coordinates of this closest point using the vertices from this triangle. 
    The barycentric coordinates are linear combination coefficients of vertices from the triangle.
    For each collider, linearly combine the support points corresponding to the vertices of the triangle, using the same barycentric coordinates as coefficients.
    This gives us contact points on each collider in local space. We can then convert these contact points to world space.
End EPA.
*/

/*


            glm::vec3 n_abc = glm::cross(b - a, c - a);
            glm::vec3 n_bdc = glm::cross(d - b, c - b);
            glm::vec3 n_acd = glm::cross(c - a, d - a);
            glm::vec3 n_adb = glm::cross(d - a, b - a);
            
            // we are checking if these faces normals are pointing away from origo, notice -b in bdc case
            if (glm::dot(n_abc, -a) > 0.0f) printf("abc outward: %.3f\n", glm::dot(n_abc, -a));
            if (glm::dot(n_bdc, -b) > 0.0f) printf("bdc outward: %.3f\n", glm::dot(n_bdc, -b));
            if (glm::dot(n_acd, -a) > 0.0f) printf("acd outward: %.3f\n", glm::dot(n_acd, -a));
            if (glm::dot(n_adb, -a) > 0.0f) printf("adb outward: %.3f\n", glm::dot(n_adb, -a));
            
*/