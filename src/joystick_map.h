#include "libretro.h"
#include "godot_cpp/classes/global_constants.hpp"

unsigned godotButtonToRetroKey(godot::JoyButton joyButton) {
    switch (joyButton) {
        // D-pad
        case godot::JoyButton::JOY_BUTTON_DPAD_LEFT:
            return RETRO_DEVICE_ID_JOYPAD_LEFT;
        case godot::JoyButton::JOY_BUTTON_DPAD_RIGHT:
            return RETRO_DEVICE_ID_JOYPAD_RIGHT;
        case godot::JoyButton::JOY_BUTTON_DPAD_UP:
            return RETRO_DEVICE_ID_JOYPAD_UP;
        case godot::JoyButton::JOY_BUTTON_DPAD_DOWN:
            return RETRO_DEVICE_ID_JOYPAD_DOWN;

        // Action Buttons (PS4: X, O, Square, Triangle)
        case godot::JoyButton::JOY_BUTTON_A: // PS4: X
            return RETRO_DEVICE_ID_JOYPAD_A;
        case godot::JoyButton::JOY_BUTTON_B: // PS4: O
            return RETRO_DEVICE_ID_JOYPAD_B;
        case godot::JoyButton::JOY_BUTTON_X: // PS4: Square
            return RETRO_DEVICE_ID_JOYPAD_X;
        case godot::JoyButton::JOY_BUTTON_Y: // PS4: Triangle
            return RETRO_DEVICE_ID_JOYPAD_Y;

        // Shoulder Buttons (PS4: L1, R1)
        case godot::JoyButton::JOY_BUTTON_LEFT_SHOULDER: // PS4: L1
            return RETRO_DEVICE_ID_JOYPAD_L;
        case godot::JoyButton::JOY_BUTTON_RIGHT_SHOULDER: // PS4: R1
            return RETRO_DEVICE_ID_JOYPAD_R;

        // // Trigger Buttons (PS4: L2, R2)
        // case godot::JoyButton::JOY_BUTTON_L: // PS4: L2
        //     return RETRO_DEVICE_ID_JOYPAD_L2;
        // case godot::JoyButton::JOY_BUTTON_R2: // PS4: R2
        //     return RETRO_DEVICE_ID_JOYPAD_R2;

        // // Additional Buttons (PS4: L3, R3, Options, Share, etc.)
        // case godot::JoyButton::JOY_BUTTON_L3: // PS4: L3
        //     return RETRO_DEVICE_ID_JOYPAD_L3;
        // case godot::JoyButton::JOY_BUTTON_R3: // PS4: R3
        //     return RETRO_DEVICE_ID_JOYPAD_R3;
        // case godot::JoyButton::JOY_BUTTON_START: // PS4: Options
        //     return RETRO_DEVICE_ID_JOYPAD_START;
        // case godot::JoyButton::JOY_BUTTON_SELECT: // PS4: Share
        //     return RETRO_DEVICE_ID_JOYPAD_SELECT;

        // // Touchpad (PS4)
        // case godot::JoyButton::JOY_BUTTON_TOUCHPAD:
        //     return RETRO_DEVICE_ID_JOYPAD_TOUCHPAD;

        default:
            return 0; // If the button is not mapped, return 0 or another error value.
    }
}
