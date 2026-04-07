#pragma once


namespace Input{
    enum class Key : uint8_t {
        Forward, Backward, Left, Right, Jump, Sprint, LeftClick, RightClick,  COUNT
    };

    struct State {
            // Bitsets: 1 bit per key. (current & ~previous) = Just Pressed.
            uint64_t current{0};
            uint64_t previous{0};

            // Mouse Deltas: Displacement since last frame.
            float mouseDX{0.0f};
            float mouseDY{0.0f};

            inline bool keyPressed(Key k){
                uint64_t mask = (1ULL << static_cast<uint8_t>(k));
                return (current & mask) && !(previous & mask); 
            }
            inline bool keyHeld(Key k){
                uint64_t mask = (1ULL << static_cast<uint8_t>(k));
                return (current & mask);
            }
            inline bool keyReleased(Key k){
                uint64_t mask = (1ULL << static_cast<uint8_t>(k));
                return !(current & mask) && (previous & mask); 
            }
        };

};
