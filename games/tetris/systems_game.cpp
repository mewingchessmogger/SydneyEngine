
#include "game.hpp"

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

enum AKAnim : int {
    Equip        = 0,
    AimFire      = 1,
    Fire         = 2,
    Hold         = 3,
    Idle         = 4,
    IdleAim      = 5,
    Reload       = 6,
    Walk         = 7,
    WalkAim      = 8,
    Run          = 9,
    NaturalPose  = 10
};

struct AnimParams {
    bool isMoving         = false; // WASD held
    bool isRunning = false; // shift held 
    bool isAiming = false;
    bool isReloading = false;
    bool isFiring = false;
    
};


struct Transition {
    AKAnim dst;
    bool (*condition)(const AnimParams&);
    bool lockNext; // does entering 'dst' lock the state?
    bool bypass;    // can this edge override a locked current state?
    void (*print)() =nullptr;
};

bool condMove(const AnimParams& p) { return p.isMoving && !p.isRunning && !p.isAiming;}
bool condStop(const AnimParams& p) { return !p.isMoving && !p.isAiming; }
bool condRun(const AnimParams& p) { return p.isMoving && p.isRunning; }
bool condIdleAim(const AnimParams& p) {return p.isAiming && !p.isMoving && !p.isFiring;}
bool condWalkAim(const AnimParams& p) {return p.isAiming && p.isMoving && !p.isFiring;}
bool condReload(const AnimParams& p) {return p.isReloading;}
bool condFiring(const AnimParams& p) {return p.isFiring && !p.isRunning && !p.isAiming;}
bool condAimFiring(const AnimParams& p) {return p.isFiring && !p.isRunning && p.isAiming;}
void goon() {printf("DIVEJOB\n");}

constexpr Transition fromIdle[] = {
    {AKAnim::Walk, condMove, false, true, goon},
    {AKAnim::Run, condRun, false, true },
    {AKAnim::IdleAim, condIdleAim, false, true },
    {AKAnim::Reload, condReload, true, true },
    
};

constexpr Transition fromWalk[] = {
    { AKAnim::Idle, condStop, false, true},
    { AKAnim::Run, condRun, false, true },
    {AKAnim::IdleAim, condIdleAim, false, true },
    {AKAnim::WalkAim, condWalkAim, false, true },
    {AKAnim::Reload, condReload, true, true },
};

constexpr Transition fromRun[] = {
    {AKAnim::Walk, condMove, false, true },
    {AKAnim::Idle, condStop, false, true },
    {AKAnim::IdleAim, condIdleAim, false, true },
};


constexpr Transition fromIdleAim[] = {
    {AKAnim::Walk, condMove, false, true },
    {AKAnim::Idle, condStop, false, true },
    //{AKAnim::Run, condRun, false, true },
    {AKAnim::WalkAim, condWalkAim, false, true },
    {AKAnim::Reload, condReload, true, true },
    {AKAnim::AimFire, condAimFiring, true, true },
};

constexpr Transition fromWalkAim[] = {
    {AKAnim::Walk, condMove, false, true },
    {AKAnim::Idle, condStop, false, true },
    //{AKAnim::Run, condRun, false, true },
    {AKAnim::IdleAim, condIdleAim, false, true },
    {AKAnim::Reload, condReload, true, true },
    {AKAnim::AimFire, condAimFiring, true, true },
    

};

constexpr Transition fromReload[] = {
    {AKAnim::Walk, condMove, false, false },
    {AKAnim::Idle, condStop, false, false},
    {AKAnim::Run, condRun, false, false},
    {AKAnim::IdleAim, condIdleAim, false, false},

};

constexpr Transition fromFire[] = {
    {AKAnim::Walk, condMove, false, false },
    {AKAnim::Idle, condStop, false, false},
    {AKAnim::Run, condRun, false, false},
    {AKAnim::IdleAim, condIdleAim, false, false},
};

constexpr Transition fromAimFire[] = {
    {AKAnim::Walk, condMove, false, false },
    {AKAnim::Idle, condStop, false, false},
    //{AKAnim::Run, condRun, false, false},
    {AKAnim::IdleAim, condIdleAim, false, false},
    {AKAnim::WalkAim, condWalkAim, false, true },
    {AKAnim::AimFire, condAimFiring, true, true },

};



struct StateTransitions {
    AKAnim state;
    const Transition* transitions;
    size_t count;
};

constexpr StateTransitions stateTable[] = {
    { AKAnim::Idle, fromIdle, std::size(fromIdle) },
    { AKAnim::Walk, fromWalk, std::size(fromWalk) },
    { AKAnim::Run, fromRun, std::size(fromRun) },
    { AKAnim::IdleAim, fromIdleAim, std::size(fromIdleAim) },
    { AKAnim::WalkAim, fromWalkAim,std::size(fromWalkAim) },
    { AKAnim::Reload, fromReload,std::size(fromReload) },
    { AKAnim::Fire, fromFire,std::size(fromFire) },
    { AKAnim::AimFire, fromAimFire,std::size(fromAimFire) },



};

void updateWeaponSystem(Input::State& state, EngineAPI& api, ECS::Registry& reg){
    constexpr static std::array<std::string_view, 11> anims = {
    "RIG_UE5_Comando_AK_Equip",        // 0
    "RIG_UE5_Comando_AK_Aim_Fire",    // 1
    "RIG_UE5_Comando_AK_Fire",        // 2
    "RIG_UE5_Comando_AK_Hold",        // 3
    "RIG_UE5_Comando_AK_Idle",        // 4
    "RIG_UE5_Comando_AK_Idle_Aim",    // 5
    "RIG_UE5_Comando_AK_Reload",      // 6
    "RIG_UE5_Comando_AK_Walk",        // 7
    "RIG_UE5_Comando_AK_Walk_Aim",    // 8
    "RIG_UE5_Comando_AK__Run",        // 9
    "RIG_UE5_Comando_Natural_pose"   // 10
};

    if(!reg.getPool<Animated>().dense.size()){
        return;
    }
    //static int CR_STATE i = 3;
    Weapon& wpn = reg.getPool<Weapon>().data[0];
    auto& anim = reg.getPool<Animated>().get(wpn.id);
    
    AnimParams p{};
    p.isMoving = state.keyHeld(Input::Key::Z) || state.keyHeld(Input::Key::X) || state.keyHeld(Input::Key::C) || state.keyHeld(Input::Key::V);
    p.isRunning = state.keyHeld(Input::Key::LeftShift);
    p.isAiming = state.keyHeld(Input::Key::MouseRight);
    p.isReloading = state.keyPressed(Input::Key::R);
    p.isFiring = state.keyHeld(Input::Key::MouseLeft);
    AKAnim current = static_cast<AKAnim>(anim.animationIndex);

    for (auto& st : stateTable){
        if (st.state != current) {continue;} // skip states not current
        for(int i = 0; i < st.count; i++){
            const Transition& t = st.transitions[i]; // get current state
            if (t.condition(p) && current != t.dst){ // if pass conditons and we are not playing the same anim
                if(t.print != nullptr){
                    t.print();
                }

                api.setAnimation(anims[t.dst], wpn.id, t.lockNext, t.bypass); //current state updated here
                return;
            }
        }
    }

}
