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
