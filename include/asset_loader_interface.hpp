#pragma once
//#include <string>
class IAssetLoader{
    public:
    virtual ~IAssetLoader() = default;
    virtual void loadModel(std::string path) = 0;
};