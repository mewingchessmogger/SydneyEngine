#pragma once
#include <cstdint>

namespace Input{
    enum class Key : uint8_t {
        // Alphanumeric Keys
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        
        Num0, Num1, Num2, Num3, Num4, 
        Num5, Num6, Num7, Num8, Num9,

        // Whitespace & System Navigation
        Space,
        Enter,
        Escape,
        Tab,
        Backspace,
        Delete,

        // Modifier Keys
        LeftShift,
        RightShift,
        LeftControl,
        RightControl,
        LeftAlt,
        RightAlt,

        // Directional Navigation
        ArrowUp,
        ArrowDown,
        ArrowLeft,
        ArrowRight,

        // Mouse Input Buttons
        MouseLeft,
        MouseRight,
        MouseMiddle,

        COUNT // Total count must not exceed 64 for 64-bit state masking
    };
    
    struct State {
            // Bitsets: 1 bit per key. (current & ~previous) = Just Pressed.
            uint64_t current{0};
            uint64_t previous{0};
            float lastX{};
            float lastY{};
            float mouseDX{0.0f};
            float mouseDY{0.0f};
            
            bool requestCursorVisible = false;

            inline void clearCursorDeltas(){
                mouseDX = 0;
                mouseDY = 0;
            }
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
