#include "libretro.h"
#include "godot_cpp/classes/global_constants.hpp"
#include <unordered_map>

unsigned godotButtonToRetroKey(godot::JoyButton joyButton)
{
    static const std::unordered_map<godot::JoyButton, unsigned> key_map = {
        // D-pad
        {godot::JoyButton::JOY_BUTTON_DPAD_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT},
        {godot::JoyButton::JOY_BUTTON_DPAD_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT},
        {godot::JoyButton::JOY_BUTTON_DPAD_UP, RETRO_DEVICE_ID_JOYPAD_UP},
        {godot::JoyButton::JOY_BUTTON_DPAD_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN},

        // Action Buttons (PS4: X, O, Square, Triangle)
        {godot::JoyButton::JOY_BUTTON_B, RETRO_DEVICE_ID_JOYPAD_A},   // PS4: X
        {godot::JoyButton::JOY_BUTTON_A, RETRO_DEVICE_ID_JOYPAD_B},   // PS4: O
        {godot::JoyButton::JOY_BUTTON_X, RETRO_DEVICE_ID_JOYPAD_X},   // PS4: Square
        {godot::JoyButton::JOY_BUTTON_Y, RETRO_DEVICE_ID_JOYPAD_Y},   // PS4: Triangle

        // Shoulder Buttons (PS4: L1, R1)
        {godot::JoyButton::JOY_BUTTON_LEFT_SHOULDER, RETRO_DEVICE_ID_JOYPAD_L},  // PS4: L1
        {godot::JoyButton::JOY_BUTTON_RIGHT_SHOULDER, RETRO_DEVICE_ID_JOYPAD_R}  // PS4: R1
    };

    auto it = key_map.find(joyButton);
    return (it != key_map.end()) ? it->second : RETROK_UNKNOWN;
}
