#pragma once

class EngineAPI{
    private:
    int gameCameraEntity{};
    

    

    public:
    enum Command{
      LOAD_SCENE, SAVE_SCENE, CREATE_SCENE, SET_DIR_MODELS, SET_DIR_SCENE, LOAD_MODEL, ATTACH_MODEL, SET_ANIMATION,LERP_ANIMATION, ATTACH_MESH_TO_BONE, COCONUT
    };

    struct PayloadCSI{Command cmd; std::string path{}; int EntityID = -1; };

    struct PayloadCSSI{Command cmd; std::string model{}; std::string bone{}; int EntityID = -1; }; // Command String String Integer -> CSSI


    struct Request{
        Command cmd;
        std::string path{};
        int EntityID = -1;
    };
    
    std::vector<Request> reqs{};
    
    void loadModel(const std::string path, int id = -1){
        reqs.push_back({LOAD_MODEL, path, -1});
    }

    void loadModels(const std::vector<std::string>& paths){
        for(const auto& path : paths){
            reqs.push_back({LOAD_MODEL, path, -1});
        }
    }

    void attachModel(const std::string path, int id){
        reqs.push_back({ATTACH_MODEL, path, id});
    }

    void setGameCamera(int entityID){
        gameCameraEntity = entityID;
    }
    int getGameCamera(){
        return gameCameraEntity;
    }

    void setAnimation(std::string animationName, int entityID){
        reqs.push_back({SET_ANIMATION,animationName,entityID});
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
    