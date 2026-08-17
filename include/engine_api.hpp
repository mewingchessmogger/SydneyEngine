#pragma once
//#include <variant>
//#include <vector>
#include  "string_hasher.hpp"
class EngineAPI{
    private:
    int gameCameraEntity{};
    

    public:
    enum Command{
      LOAD_SCENE, SAVE_SCENE, CREATE_SCENE, SET_DIR_MODELS, SET_DIR_SCENE, LOAD_MODEL, ATTACH_MODEL, SET_ANIMATION,LERP_ANIMATION, ATTACH_ETE_BONE, COCONUT
    };
    enum EventAction{
      COLLISION, COUNT
    };
   
    struct Request{
        Command cmd;
        std::string path{};
        int EntityID = -1;
    };
    

    struct AnimationRequest{
        Command cmd{};
        uint64_t hash{};
        int EntityID = -1;
        int layer = -1;
        float crossfade = 0;
        bool locked = false;
        bool bypassLocked = false;
    };

    struct AttachEntityToEntityRequest{
        Command cmd{};
        uint64_t hash{}; // bone
        int srcEntity = -1;
        int dstEntity = -1;
    };

    struct Event{
        EventAction event{};
        int srcEntity{-1};
        int dstEntity{-1};
    };

    std::vector<std::variant<Request,AnimationRequest, AttachEntityToEntityRequest>> reqs{};
    std::vector<Event> events{};

    void loadModel(const std::string path, int id = -1){
        reqs.push_back(Request{LOAD_MODEL, path, -1});
    }

    void loadModels(const std::vector<std::string>& paths){
        for(const auto& path : paths){
            reqs.push_back(Request{LOAD_MODEL, path, -1});
        }
    }

    void attachModel(const std::string path, int id){
        reqs.push_back(Request{ATTACH_MODEL, path, id});
    }
    
    void setGameCamera(int entityID){
        gameCameraEntity = entityID;
    }
    int getGameCamera(){
        return gameCameraEntity;
    }

    void setAnimation(AnimationRequest& req){
        reqs.push_back(req);
        //reqs.push_back({SET_ANIMATION,animationName,entityID,});
    }


    void setAnimation(std::string_view name, int entityID,bool locked = false, bool bypassLocked = false, int layer = 0, float crossfade = 0.0f) {
        
        uint64_t hash = Hasher::stringview(name);
        
        reqs.push_back(AnimationRequest{
            SET_ANIMATION, 
            hash,
            entityID,
            layer,
            crossfade,
            locked,
            bypassLocked
        });
    }
    
    void attachEntityToEntityBone(std::string_view boneName, int srcEntity, int dstEntity){
        
        uint64_t hash = Hasher::stringview(boneName);
        
        reqs.push_back(AttachEntityToEntityRequest{
            ATTACH_ETE_BONE, 
            hash,
            srcEntity,
            dstEntity
        });
    }

    /*
    void lerpAnimation(std::string animName, int entityID){
        reqs.push_back({LERP_ANIMATION,animationName,entityID});
    }
    
    void attachModelToBone(std::string& srcName, std::string& dstName, int entityID){
            reqs.push_back({ATTACH_MODEL_TO_BONE,animationName,entityID});

    }

    */
}; 
    