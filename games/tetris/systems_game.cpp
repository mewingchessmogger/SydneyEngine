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
        /*
        animation #0 'RIG_UE5_Comando_AK_Equip      ', duration (in sec): 1.833333, ticks per second: 1000.000000, it has 110 channels
    animation #1 'RIG_UE5_Comando_AK_Aim_Fire   ', duration (in sec): 0.666667, ticks per second: 1000.000000, it has 110 channels
    animation #2 'RIG_UE5_Comando_AK_Fire       ', duration (in sec): 0.666667, ticks per second: 1000.000000, it has 110 channels
    animation #3 'RIG_UE5_Comando_AK_Hold       ', duration (in sec): 0.750000, ticks per second: 1000.000000, it has 110 channels
    animation #4 'RIG_UE5_Comando_AK_Idle       ', duration (in sec): 2.500000, ticks per second: 1000.000000, it has 110 channels
    animation #5 'RIG_UE5_Comando_AK_Idle_Aim   ', duration (in sec): 2.500000, ticks per second: 1000.000000, it has 110 channels
    animation #6 'RIG_UE5_Comando_AK_Reload     ', duration (in sec): 4.583333, ticks per second: 1000.000000, it has 110 channels
    animation #7 'RIG_UE5_Comando_AK_Walk       ', duration (in sec): 1.000000, ticks per second: 1000.000000, it has 110 channels
    animation #8 'RIG_UE5_Comando_AK_Walk_Aim   ', duration (in sec): 1.000000, ticks per second: 1000.000000, it has 110 channels
    animation #9 'RIG_UE5_Comando_AK__Run       ', duration (in sec): 0.791667, ticks per second: 1000.000000, it has 110 channels
    animation #10 'RIG_UE5_Comando_Natural_pose  ', duration (in sec): 0.000000, ticks per second: 1000.000000, it has 110 channels*/

        // Static index to track animation progression
        static size_t animIndex = 0;

        if(!reg.getPool<Animated>().dense.size()){
            return;
        }

        Weapon& wpn = reg.getPool<Weapon>().data[0];
        auto& anim = reg.getPool<Animated>().get(wpn.id);

        static bool CR_STATE reloading = false;
        if(state.keyPressed(Input::Key::Jump)){
            api.setAnimation("RIG_UE5_Comando_AK_Reload", wpn.id);
            reloading = true;
            return;
        }

        if(reloading){
            if (anim.time < anim.duration* 0.98){
                return;
            }else{
                reloading  = false;
                return;
            }

        }

        if(state.keyReleased(Input::Key::RightClick)){
            api.setAnimation("RIG_UE5_Comando_AK_Idle", wpn.id);
        }
        if(state.keyPressed(Input::Key::LeftClick)){
            api.setAnimation("RIG_UE5_Comando_AK_Fire", wpn.id);
        }
        else if(state.keyHeld(Input::Key::LeftClick)){
            if (anim.time > 0.25){
                anim.time = 0.15;
            }
        
        }else if(state.keyReleased(Input::Key::LeftClick)){
            
            api.setAnimation("RIG_UE5_Comando_AK_Idle", wpn.id);
        }


        if(state.keyPressed(Input::Key::RightClick)){
            api.setAnimation("RIG_UE5_Comando_AK_Idle_Aim", wpn.id);
        }
        else if(state.keyHeld(Input::Key::RightClick)){
            if (state.keyPressed(Input::Key::LeftClick)){
                api.setAnimation("RIG_UE5_Comando_AK_Aim_Fire", wpn.id);
            }
            else if(state.keyHeld(Input::Key::LeftClick)){
                if (anim.time > 0.25){
                    anim.time = 0.1;
                }
            }
            else if(state.keyReleased(Input::Key::LeftClick)){
                api.setAnimation("RIG_UE5_Comando_AK_Idle_Aim", wpn.id);
            }
        }

        
        
        

        // if(state.keyPressed(Input::Key::LeftClick) && !state.keyPressed(Input::Key::RightClick)){
        //     api.setAnimation("RIG_UE5_Comando_AK_Fire", wpn.id);
        
        //     anim.loop = 0.30;
        //     anim.loopOffset = 0.0;
        // }

    



        // else if(state.keyHeld(Input::Key::RightClick)){
        //     if (anim.time > 2.0){
        //         anim.time = 1.8;
        //     }

        // if(state.keyPressed(Input::Key::LeftClick)){
        //     api.setAnimation("RIG_UE5_Comando_AK_Aim_Fire", wpn.id);
        
        //     anim.loop = 0.30;
        //     anim.loopOffset = 0.0;
        // }
        // else if(state.keyHeld(Input::Key::LeftClick)){
        //     printf("%f\n", anim.time);
        //     if (anim.time > 0.25){
        //         anim.time = 0.1;
        //     }
        // }
    
        // }



    }
