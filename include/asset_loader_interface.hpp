#pragma once
//#include <string>
class IAssetLoader{
    public:
    virtual ~IAssetLoader() = default;
    virtual void loadModel(std::string filename, bool  tryCaching) = 0;
};