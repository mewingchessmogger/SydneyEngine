#pragma once

class EngineAPI{
    private:
    int gameCameraEntity{};
    
    public:
    enum Command{
      LOAD_SCENE, SAVE_SCENE, CREATE_SCENE, SET_DIR_MODELS, SET_DIR_SCENE, LOAD_MODEL, ATTACH_MODEL, SET_ANIMATION, COCONUT
    };

    struct Request{
        Command cmd;
        std::string path{};
        int EntityID = -1;
    };
    
    std::vector<Request> reqs{};
    
    void loadModel(const std::string path, int id = -1){
        reqs.push_back({LOAD_MODEL, path, id});
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
    //api.setAnimation(2, "shooting")
}; 
    