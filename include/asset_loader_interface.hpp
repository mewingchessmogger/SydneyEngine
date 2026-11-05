#pragma once
//#include <string>
class IAssetLoader{
    public:
    virtual ~IAssetLoader() = default;
    virtual void loadModel(std::string filename, bool  tryCaching) = 0;
    virtual std::vector<glm::vec3> getTerrainPolygons(std::string filename) = 0;
};