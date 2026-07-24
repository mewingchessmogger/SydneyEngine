#include "game.hpp"

/*
 enum class Key : uint8_t {
        Forward, Backward, Left, Right, Jump, Sprint, LeftClick, RightClick, Escape, Quit, COUNT
    };
    
// FOR GUN MODEL AK
animation #0 'RIG_UE5_Comando_AK_Equip      ', duration (seconds): 1.833333
animation #1 'RIG_UE5_Comando_AK_Aim_Fire   ', duration (seconds): 0.666667
animation #2 'RIG_UE5_Comando_AK_Fire       ', duration (seconds): 0.666667
animation #3 'RIG_UE5_Comando_AK_Hold       ', duration (seconds): 0.750000
animation #4 'RIG_UE5_Comando_AK_Idle       ', duration (seconds): 2.500000
animation #5 'RIG_UE5_Comando_AK_Idle_Aim   ', duration (seconds): 2.500000
animation #6 'RIG_UE5_Comando_AK_Reload     ', duration (seconds): 4.583333
animation #7 'RIG_UE5_Comando_AK_Walk       ', duration (seconds): 1.000000
animation #8 'RIG_UE5_Comando_AK_Walk_Aim   ', duration (seconds): 1.000000
animation #9 'RIG_UE5_Comando_AK__Run       ', duration (seconds): 0.791667
animation #10 'RIG_UE5_Comando_Natural_pose 

*/
void updateWeaponSystem(Input::State& state, EngineAPI& api, ECS::Registry& reg){
    constexpr static std::array<const char*, 10> animNames = {
        "RIG_UE5_Comando_AK_Equip",
        "RIG_UE5_Comando_AK_Aim_Fire",
        "RIG_UE5_Comando_AK_Fire",
        "RIG_UE5_Comando_AK_Hold",
        "RIG_UE5_Comando_AK_Idle",
        "RIG_UE5_Comando_AK_Idle_Aim",
        "RIG_UE5_Comando_AK_Reload",
        "RIG_UE5_Comando_AK_Walk",
        "RIG_UE5_Comando_AK_Walk_Aim",
        "RIG_UE5_Comando_AK__Run"
    };

    // Static index to track animation progression
    static size_t animIndex = 0;
    Weapon& wpn = reg.getPool<Weapon>().data[0];

    if(state.keyPressed(Input::Key::RightClick)){
        animIndex = (animIndex + 1) % animNames.size();
        api.setAnimation(animNames[animIndex], wpn.id);
        
        
        glm::vec<3,float> v = reg.getPool<Transform>().get(wpn.id).position;
        printf("%f, %f, %f \n", v.x, v.y, v.z);
    }else if(state.keyPressed(Input::Key::LeftClick)){
        api.setAnimation("RIG_UE5_Comando_AK_Reload", wpn.id);
    }
}
