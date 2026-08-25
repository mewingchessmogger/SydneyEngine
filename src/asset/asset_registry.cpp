#include "asset_registry.hpp"


AssetRegistry::StaticModel &AssetRegistry::getStaticModelFromID(uint32_t id)
{
    auto& stringMap = IntegerToStringStaticModelMap;
    auto& mdlMap = staticModelMap;
    
    if(stringMap.find(id) == stringMap.end()){
        printf("NO ID TO STRING EXISTS THISS ONE STATIC!!! \n");
        //throw std::runtime_error("NO ID TO STRING EXISTS THISS ONE ");
    }

    return mdlMap[stringMap[id]];
}



AssetRegistry::SkinnedModel &AssetRegistry::getSkinnedModelFromID(uint32_t id)
{
    auto& stringMap = IntegerToStringSkinnedModelMap;
    auto& mdlMap = skinnedModelMap;
    if(stringMap.find(id) == stringMap.end()){
        printf("NO ID TO STRING EXISTS THISS ONE SKINNED!!! \n");
    }
    return mdlMap[stringMap[id]];
}




uint32_t AssetRegistry::getModelID(std::string& path)
{
    if(StringToIntegerSkinnedModelMap.find(path) != StringToIntegerSkinnedModelMap.end()){
        return StringToIntegerSkinnedModelMap[path];
    }else{
     if(StringToIntegerStaticModelMap.find(path) != StringToIntegerStaticModelMap.end()){
        return StringToIntegerStaticModelMap[path];
    }   
    assert(0); // should have never come here...
    }
}

uint32_t AssetRegistry::getModelID(std::string&& path)
{
    return getModelID(path);
}

AssetRegistry::StaticModel &AssetRegistry::getStaticModelFromString(std::string&& path)
{
    uint32_t id = getModelID(path);
    return getStaticModelFromID(id);

}



