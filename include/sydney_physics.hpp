#pragma once 

#include "glm/matrix.hpp"      
#include <glm/gtx/norm.hpp>
#include "reflections.hpp"

#include "engine_components.hpp"
#include <limits>
#include <utility>
#include "sydney_physics_components.hpp"
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
   

    struct CandidatePair{
        int A{};
        int B{};
        vec3 n{};
        float depth{};
    };
    struct CollisionCandidate {
        std::array<ECS::Entity, 2> entities;
        std::array<glm::vec3, 4> simplexPoints; 
    };

    

    inline bool checkMidPhase(Collider& c1, Collider& c2, glm::mat4& r1, glm::mat4& r2){
        float rSquared = (c1.broadRadius + c2.broadRadius) * (c1.broadRadius + c2.broadRadius);
        return rSquared > glm::distance2(glm::vec3(r1[3])+ c1.offset, glm::vec3(r2[3]) + c2.offset);
    }
    inline bool checkMidPhaseRB(const Collider& c1, const Collider& c2,const glm::vec3& pos1 , const glm::vec3& pos2){
        float rSquared = (c1.broadRadius + c2.broadRadius) * (c1.broadRadius + c2.broadRadius);
        return rSquared > glm::distance2(pos1+ c1.offset, pos2 + c2.offset);
    }

    
    inline glm::vec3 support(glm::vec3 dir, Collider& c1, RawTransform& r1){
            assert(c1.narrowShape != c1.NONE);
            dir = normalize(dir);
            switch(c1.narrowShape){

                case Collider::SPHERE:{
                    return vec3(c1.narrowRadius)* normalize(dir) + c1.offset  + r1.getPosition();
                }
                case Collider::AABB: {
                    vec3 extents = c1.narrowExtents;
                    return vec3(
                        (dir.x < 0) ? -extents.x : extents.x,
                        (dir.y < 0) ? -extents.y : extents.y,
                        (dir.z < 0) ? -extents.z : extents.z
                    ) + c1.offset + r1.getPosition();

                }
                case Collider::OBB: {
                    vec3 extents = c1.narrowExtents;
                    glm::mat3 R = r1.getRotationMatrix();
                    vec3 rotated = glm::transpose(R) * dir;
                    
                    vec3 localSupport{
                        (rotated.x < 0) ? -extents.x : extents.x,
                        (rotated.y < 0) ? -extents.y : extents.y,
                        (rotated.z < 0) ? -extents.z : extents.z
                    };

                    return R * localSupport + c1.offset + r1.getPosition();

                }
            }
        }
    
    inline glm::vec3 supportRawTrans(glm::vec3 dir, Collider& c1, RawTransform& r1){
            assert(c1.narrowShape != c1.NONE);
            dir = normalize(dir);
            switch(c1.narrowShape){

                case Collider::SPHERE:{
                    return vec3(c1.narrowRadius)* normalize(dir) + c1.offset  + r1.getPosition();
                }
                case Collider::AABB: {
                    vec3 extents = c1.narrowExtents;
                    return vec3(
                        (dir.x < 0) ? -extents.x : extents.x,
                        (dir.y < 0) ? -extents.y : extents.y,
                        (dir.z < 0) ? -extents.z : extents.z
                    ) + c1.offset + r1.getPosition();

                }
                case Collider::OBB: {
                    vec3 extents = c1.narrowExtents;
                    glm::mat3 R = r1.getRotationMatrix();
                    vec3 rotated = glm::transpose(R) * dir;
                    
                    vec3 localSupport{
                        (rotated.x < 0) ? -extents.x : extents.x,
                        (rotated.y < 0) ? -extents.y : extents.y,
                        (rotated.z < 0) ? -extents.z : extents.z
                    };

                    return R * localSupport + c1.offset + r1.getPosition();

                }
            }
    }
    inline glm::vec3 supportRigidBody(glm::vec3 dir, Collider& c1, RigidBody& r1){
            assert(c1.narrowShape != c1.NONE);
            dir = normalize(dir);
            switch(c1.narrowShape){

                case Collider::SPHERE:{
                    return vec3(c1.narrowRadius)* normalize(dir) + c1.offset  + r1.position;
                }
                case Collider::AABB: {
                    vec3 extents = c1.narrowExtents;
                    return vec3(
                        (dir.x < 0) ? -extents.x : extents.x,
                        (dir.y < 0) ? -extents.y : extents.y,
                        (dir.z < 0) ? -extents.z : extents.z
                    ) + c1.offset + r1.position;

                }
                case Collider::OBB: {
                    vec3 extents = c1.narrowExtents;
                    glm::mat3 R = glm::mat3_cast(r1.rotation);
                    vec3 rotated = glm::transpose(R) * dir;
                    
                    vec3 localSupport{
                        (rotated.x < 0) ? -extents.x : extents.x,
                        (rotated.y < 0) ? -extents.y : extents.y,
                        (rotated.z < 0) ? -extents.z : extents.z
                    };

                    return R * localSupport + c1.offset + r1.position;

                }
            }
    }

    //will support hierarchical chilren colliders
    inline glm::vec3 supportA_minus_B(glm::vec3 dir, Collider& c1, Collider& c2, RawTransform& r1, RawTransform& r2){
        return supportRawTrans(dir, c1, r1) - supportRawTrans(-dir,c2,r2);
    }

    //only supports ROOT entities, rigidbodyes cant be children
    inline glm::vec3 supportA_minus_B_RigidBody(glm::vec3 dir, Collider& c1, Collider& c2, RigidBody& r1, RigidBody& r2){
        return supportRigidBody(dir, c1, r1) - supportRigidBody(-dir,c2,r2);
    }

        
    struct GJK{
        
        vec3 dir;
        vec3 a, b, c, d;
        uint32_t n{}; // n refers to the amount of stored points excluding a!
    
    struct ResultGJK{
        std::array<glm::vec3,4> abcd{};
        bool intersected{};
    };
        bool update(){
           
            switch(n){

                case 0: { 
                    zero_case:
                    b = a;
                    dir = -a;
                    n = 1;
                    return false;
                }
                case 1:{ 
                    line_case:

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
                case 2: { 
                    triangle_case:

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
                            goto line_case;  //alternative to do return recursive update() call
                        }
                        
                    }
                    else{
                        if(dot(cross(ab, norm_abc), ao) > 0){
                            n = 1;
                            goto line_case;
                        }
                        else{
                            //imagine a triangular infinte pillar going through the triangles in the direciton of its normal, we have determined that origio is inside the pillar, 
                            n = 3;
                            
                            d = c;
                            c = b;
                            b = a;

                            /*convince yourself that using this triangle which normal WILL point towards origo,
                             with that choice, the future three faces will ALWAYS have a normal that points OUTWARDS from ORIGO !!!IF we did enclose it properly!!!,  SUBTLE SHIT*/
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
                        goto triangle_case;
                    }
                    if (dot(acd,ao) > 0){
                        n = 2;
                        b = c;
                        c = d;
                        goto triangle_case;
                    }
                    if (dot(adb,ao) > 0){
                        n = 2;
                        c = b;
                        b = d;
                        //printf("adb\n");
                        goto triangle_case;
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
        ResultGJK intersect(supportFunc&& supportFn){
            
            dir = vec3{1.0, 0.0, 0.0}; 
           
            for (int i {}; i < 32; i++){
                a = supportFn(dir);
            
                if(dot(a, dir) < 0.0){
                    reset();
                    return {{}, false};
                }
                
                if (update()){
                    std::array<vec3,4> points = {a,b,c,d};
                    reset();
                    return {points, true};
                }
            }
            reset();
            return {{}, false};
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
        

        struct FaceResult{
            vec3 normal{};
            float distance{};
            int dyingFace{};
        };
        struct Edge{
            vec3 A{};
            vec3 B{};
        };
        struct ResultEPA{
                vec3 normal{};
                float depth{};
            };
        template <typename supportFunc>
        ResultEPA run(std::array<vec3,4> points, supportFunc&& supportFn){
            auto [a, b, c, d] = points;
            std::vector<std::array<vec3,3>> polytope = {{a,b,c}, {b,d,c}, {a,c,d}, {a,d,b}};        /*WELL ITS ACTULALLY CLEARED HERE SO... HAHA*/
            std::vector<Edge> uniqueEdges{};
            
            float minDistance = -std::numeric_limits<float>::infinity(); /*its MINUS INFINITY*/

            while(true){

                FaceResult result = getClosestFace(polytope); /*3. Pick the closest face of the polytope to the origin.*/

                if((result.distance - minDistance) > 0.001f){ /*4. If the closest face is no closer (by a certain threshold) to the origin than the previously picked one, return*/
                    return {result.normal, result.distance};
                }
                minDistance = result.distance;
                
                std::swap(polytope[result.dyingFace], polytope.back()); /*5. Remove the closest face, */
                polytope.pop_back();
                

                vec3 supportPoint = supportFn(result.normal); /*use the face normal (outward pointing) as the search direction to find a support point on the CSO.*/

                /*. Remove all faces from the polytope that can be “seen” by this new support point,
                and add new faces to cover up the “hole” on the polytope,
                where all new faces share the new support point as a common vertex (this is the expanding part of the algorithm).*/
                
                expand(polytope, uniqueEdges, supportPoint);
            }
            
            return {};

        }
        void expand(std::vector<std::array<vec3,3>>& polytope,std::vector<Edge>& edges, vec3 point){
            for(int i = 0; i < polytope.size();){
                auto& face = polytope[i];
                vec3 v01 = face[1] - face[0];
                vec3 v02 = face[2] - face[0];
                vec3 n = cross(v01,v02);
                if(dot(n,(point - face[0])) > 0.0f){ /*face can be "seen" by point*/  
                    addUniqueEdge(edges, face[0], face[1]);// add or remove edges accordinlgy 3 times, order inside the edgelist doesnt matter, but the points order insid each Edge struct does matter striving for CCW
                    addUniqueEdge(edges, face[1], face[2]);
                    addUniqueEdge(edges, face[2], face[0]);

                    std::swap(polytope.back(), polytope[i]);//remove face
                    polytope.pop_back();
                }else{
                    i++;
                }
            }
            for (auto& edge : edges) {
                polytope.push_back({ edge.A, edge.B, point });
            }
            edges.clear();

        }

        void addUniqueEdge(std::vector<Edge>& edges, vec3 v0, vec3 v1){
            for(int e = 0; auto& egg : edges){
                if (v0 == egg.B && v1 == egg.A){
                    std::swap(edges.back(), edges[e]);
                    edges.pop_back();
                    return;
                }
                e++;
            }
                edges.push_back({v0,v1});

        }

        FaceResult getClosestFace(std::vector<std::array<vec3,3>>& polytope){
            float minDistance = std::numeric_limits<float>::infinity();
            vec3 minNormal{};
            int faceToBeExecuted{};

            for(int i = 0; auto& face : polytope){
                vec3 v0 = face[0]; vec3 v1 = face[1]; vec3 v2 = face[2];
                vec3 n = normalize(cross(v1-v0, v2-v0));
                float d = dot(v0,n); 
                if(d < minDistance){
                    minDistance = d;
                    minNormal = n;
                    faceToBeExecuted = i;
                }
                i++;
            }
            
            return {minNormal, minDistance, faceToBeExecuted};
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
