#pragma once

class EngineAPI{
    public:
    enum Command{
      LOAD_SCENE,SAVE_SCENE,CREATE_SCENE,SET_DIR_MODELS,SET_DIR_SCENE,LOAD_MODEL,ATTACH_MODEL,
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

}; 
    