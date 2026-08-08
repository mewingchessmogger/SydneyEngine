#pragma once 
#include "glm/mat4x4.hpp"
namespace Sydphys {
    using real = float;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    enum ShapeType{
        CUBE, ORB, COUNT
    };
   
    class Particle {
    public:
        real inverseMass{};
        vec3 vel{};
        vec3 acc{};
        real damping{};
    };

    // struct ColliderView{
    //     glm::mat4& trans{};
    //     std::variant<vec3, real> params{};
    //     ShapeType& type{};
    // };
    
    // struct Simplexes{
    //     std::array<vec3,4> points{};
    //     int size{};
        
    //     void appendPoint(vec4 point){
    //         assert(size <= 4);
    //         points[size] = point;
    //         size++;
    //     }
        
    // };
    

    // inline vec3 center(ColliderView& c){
    //     switch(c.type){
    //     case(CUBE): {
    //         vec3 minCorner = vec3(-1.0f);
    //         vec3 maxCorner = vec3(1.0f);
    //         return vec3(c.trans * vec4((minCorner + maxCorner) / 2.0f),1.0f);
    //     }
       
    //     default: 
    //         throw std::runtime_error("'center()' invalid shape type");
    //         break;

    // }
    //     return {};
    // }
        
    // inline  vec3 furthestPoint(ColliderView& c, vec3 d){
    //     vec3 furthestPoint{};
    //     switch(c.type){
    //         case(CUBE): {

    //             vec3 minCorner = vec3(-1.0f);
    //             vec3 maxCorner = vec3(1.0f);
    //             float x = (d.x >= 0) ? maxCorner.x : minCorner.x;
    //             float y = (d.y >= 0) ? maxCorner.y : minCorner.y;
    //             float z = (d.z >= 0) ? maxCorner.z : minCorner.z;
    //             furthestPoint = {x,y,z};
    //             break;
    //         }
    //         case(ORB):{
    //             real radius = std::get<real>(c.params);
    //             vec3 normalizedDir = glm::normalize(d);
    //             furthestPoint = normalizedDir  * radius;
    //             break;
    //         }
    //         default:
    //             throw std::runtime_error("'furthestPoint()' invalid shape type");
    //             break;
    //     }

    //     return vec3(c.trans * vec4(furthestPoint, 1.0));
    // }
  
    // inline vec3 supportFn(ColliderView& c1, ColliderView& c2, vec3 d){
    //     return furthestPoint(c1, d) - furthestPoint(c2, -d); //max⁡{D→⋅(A−B)}
    // }

    // bool Line(Simplexes& simplex, vec3& dir){
    //     vec3 B = simplex.points[0]; // first point is b 
    //     vec3 A = simplex.points[1]; // A was checkd aftger is direction of b and after origo
    //     vec3 AB = B-A;
    //     vec3 AO = vec3(0) - A;
    //     vec3 ABCrossAO = glm::cross(AB, AO);
    //     vec3 ABPerp = glm::cross(ABCrossAO,AB);
    //     bool greater = glm::dot(AB, AO) > 0;


    //     if(greater){
    //         dir = ABPerp;
    //     }
    //     /*
    //     in beginning of the while loop, we add a point and check if its past origin,if line is called after this, those points wont trigger this "else"
    //     , in triangle case, wee call create points, and call line, those might trigger this, i tweaked over this "else" for 2 hours..
    //     */
    //     else{ 
    //         simplex.points = {A};
    //         simplex.size = 1;
    //         dir = AO;

    //     }
    //     // if ab is crossing AO
    //     // if(dot(ABCrossAO,ABCrossAO) <= 0.01)){
    //     //     return true;
    //     // }

    //     return false;

    // }
    // bool Triangle(Simplexes& simplex, vec3& dir){
    //     vec3 A = simplex.points[2]; 
    //     vec3 B = simplex.points[1]; 
    //     vec3 C = simplex.points[0];
    //     vec3 AB = B - A;
    //     vec3 AC = C - A;
    //     vec3 AO = vec3(0) - A;
    //     vec3 ABC = glm::cross(AB, AC);
        
        
    //     vec3 AB_T = glm::cross(glm::cross(AC, AB),AB); // CA Perpendicualr vector towards possible region R_AC and R_C
    //     vec3 AC_T = glm::cross(glm::cross(AB, AC),AC); // CA Perpendicualr vector towards possible region R_AB and R_B



    //     if(glm::dot(AC_T, AO) > 0){ //is origo in R_AC or R_C?  
    //         if(glm::dot(AC, AO) > 0){ // this ensures that origo is in R_AC
    //             simplex.points = {A,C}; // discard B
    //             dir = glm::cross(glm::cross(AC,AO),AC); 
    //         }
    //         else{ // if this its not in R_AC! its Region to the Right
    //             simplex.points = {A,B};
    //             simplex.size = 2;
    //             return Line(simplex,dir);
    //         }
    //     }
    //     else{ // ok it wasnt in R_AC.. check R_AB
            
    //         if(glm::dot(AB_T, AO) > 0){ //is origo in R_AB or R_B?  
    //             if(glm::dot(AB, AO) > 0){ // this ensures that origo is in R_AB
    //                 simplex.points = {A,B}; // discard C
    //                 simplex.size = 2;
    //                 dir = glm::cross(glm::cross(AB,AO),AB); 
    //             }
    //             else{ 
    //                 if(glm::dot(ABC, AO) > 0){
    //                     dir = ABC;
    //                 }
    //                 else{
    //                     simplex.points = {A,C,B};
    //                     dir = -1.0f * ABC;
    //                 }
    //                 simplex.points = {A,C};
    //                 simplex.size = 2;
    //                 return Line(simplex,dir);
    //             }
    //         }
        



    //     }if (glm::dot(AB_T, AO) > 0){
    //         simplex.points = {A,C};
    //         simplex.size = 2;
    //         dir = AB_T;
    //         return false;
    //     }
    //     else if(glm::dot(AC_T, AO) > 0.0f){
    //         simplex.points = {A,B};
    //         simplex.size = 2;
    //         dir = AC_T;
    //         return false;
    //     }
    // }

        
    
    // bool handleSimplex(Simplexes& simplex, vec3& dir){
    //     switch(simplex.size){
    //         case 2: return Line        (simplex,dir);
    //         case 3: return Triangle   (simplex,dir);
    //         case 4: return Tetrahedron(simplex, dir);
    //         default: 
    //             break;
    //     }   
    //     return false;
        
    // }


    // //https://winter.dev/articles/gjk-algorithm/
    // inline void GJK(ColliderView& c1, ColliderView& c2, bool &result, Simplexes& simplex){

    //     //get both id transofrms
    //     //get shape type
    //     vec3 d  = glm::normalize(center(c1) - center(c2));
    //     simplex.appendPoint(supportFn(c1,c2,d));
    //     d = vec3(0.0f) - simplex[0]; // origo - first    

    //     while (true){
    //         vec3 A = supportFn(c1,c2,d);
    //         if (glm::dot(A,d) < 0) 
    //             return false;
            
    //         simplex.appendPoint(A);

    //         if (handleSimplex(simplex, d))
    //             return true;
            
    //     }
    // }    
 

  
};