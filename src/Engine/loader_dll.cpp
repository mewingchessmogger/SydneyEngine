
#include "loader_dll.hpp"
#include <stdexcept>
HMODULE LoaderDLL::loadDLL(const char* nameDLL){
    //SetDllDirectoryW(L".\\games");
    HMODULE dllHandle = LoadLibraryA(nameDLL);
    
    if (!dllHandle) {
        throw std::runtime_error("Failed to load DLL\n");  
    }

    return dllHandle;
}

// IScript* LoaderDLL::acquireScriptPtr(HMODULE handle, const char* factoryFunctionName){
//     scriptPtr factory = reinterpret_cast<scriptPtr>(GetProcAddress(handle, factoryFunctionName));

//     if (!factory){
//         FreeLibrary(handle);
//         throw std::runtime_error("Failed to locate factory function\n");  
//     }
//     return factory();
// }


LoaderDLL::voidPtr LoaderDLL::getGameContextPtr(HMODULE handle, const char* factoryFunctionName){
    voidPtr factory = reinterpret_cast<voidPtr>(GetProcAddress(handle, factoryFunctionName));

    if (!factory){
        FreeLibrary(handle);
        throw std::runtime_error("Failed to locate factory function\n");  
    }
    return factory;
}

    // 6. Create an entity and push the script component straight into the pool
    // (Uses your upgraded move-only perfect forwarding add function)
    // int tetrisEntity = reg.createEntity();
    // reg.add(tetrisEntity, std::move(scriptComponent));


// load dll and acquire its handle
//get procadress of DLL's factoryFunction and call it to get game script instance

// create new script component and set its ptr to adress of script instance



// if dll is old, pass void ptr to engine, hot reload dll, pass back ptr to dll

// define g

