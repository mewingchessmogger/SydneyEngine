#include "game.hpp"
#include "windows.h"

#ifdef _WIN32
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
    DLL_EXPORT IScript* CreateScriptInstance(){
        return new Tetris();
    }
}