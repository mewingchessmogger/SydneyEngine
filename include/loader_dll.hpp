#pragma once
#include "engine_components.hpp"
#include "windows.h"

class LoaderDLL{
    
    
    public:
    //Script* getScriptFromDLL()
    using scriptPtr = IScript* (*)(); // (*) can be sen as the variable name
    HMODULE loadGameDLL(const char *nameDLL);
    IScript* acquireScriptPtr(HMODULE handle, const char *factoryFunctionName);


};

