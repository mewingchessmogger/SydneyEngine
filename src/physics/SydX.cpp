#include "sydney_physics.hpp"
namespace SydX{

    bool checkMidPhase(Collider& c1, Collider& c2, glm::mat4& r1, glm::mat4& r2){
        float rSquared = (c1.broadRadius + c2.broadRadius) * (c1.broadRadius + c2.broadRadius);
        return rSquared > glm::distance2(glm::vec3(r1[3])+ c1.offset, glm::vec3(r2[3]) + c2.offset);
    }
    glm::vec3 support(glm::vec3 dir, Collider& c1, RawTransform& r1){
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
        
    glm::vec3 supportA_minus_B(glm::vec3 dir, Collider& c1, Collider& c2, RawTransform& r1, RawTransform& r2){
        return support(dir, c1, r1) - support(-dir,c2,r2);
    }

        
};