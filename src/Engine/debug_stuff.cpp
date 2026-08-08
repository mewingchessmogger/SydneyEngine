#include "engine.hpp"

bool Engine::debugStuff(PlatformGLFW& plt, EngineMode& mode, AssetRegistry& ast){
      if(plt.inputState.keyPressed(Input::Key::Escape)){
            mode = (mode == EngineMode::GAME) ? EngineMode::EDITOR : EngineMode::GAME;
            if(mode == EngineMode::GAME){
                plt.inputState.requestCursorVisible = false;
            }else{
                plt.inputState.requestCursorVisible = true;
            }
        }
        if(mode == EngineMode::EDITOR){
            if(plt.inputState.keyHeld(Input::Key::MouseLeft)){
                plt.inputState.requestCursorVisible = false;
            }
            if(plt.inputState.keyReleased(Input::Key::MouseLeft)){
                plt.inputState.requestCursorVisible = true;
            }
        }
        

        if(plt.inputState.keyPressed(Input::Key::L)){
            return true;
        }
        if(plt.inputState.keyPressed(Input::Key::P)){
            for (auto& [name, mdl] : ldr.getAssetReg().skinnedModelMap){
                printf("\n\nModel: %s \n\n", name.c_str());
                int i{};
                for(auto& boneName : mdl.boneNames){
                    printf("        Bone %d, Name: %s\n", i++,boneName.c_str());
                }
                printf("End!!\n");
            }
        }
        return false;
}
