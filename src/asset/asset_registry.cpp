#include "asset_registry.hpp"


AssetRegistry::StaticModel &AssetRegistry::getStaticModelFromID(uint32_t id)
{
    auto& stringMap = IntegerToStringStaticModelMap;
    auto& mdlMap = staticModelMap;
    if(stringMap.find(id) == stringMap.end()){
        printf("O ID TO STRING EXISTS THISS ONE \n");
        //throw std::runtime_error("NO ID TO STRING EXISTS THISS ONE ");
    }

    return mdlMap[stringMap[id]];
}
