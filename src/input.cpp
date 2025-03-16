#include "gdretro.h"
#include "godot_cpp/classes/input_event_key.hpp"
#include "keyboard_map.h"

#include "godot_cpp/classes/input_event_joypad_button.hpp"
#include "joystick_map.h"
#include "godot_cpp/classes/input_event_joypad_motion.hpp"

#include "godot_cpp/variant/utility_functions.hpp"
#include "loader.h"

static bool keyboard_state[RETROK_LAST] = { false };

static bool joypad_state[RETRO_DEVICE_ID_JOYPAD_R3 + 1] = { false };


void GDRetro::core_input_poll( void )
{
    // TODO
    // godot::UtilityFunctions::print( "Input poll" );
}

int16_t GDRetro::core_input_state( unsigned port, unsigned device, unsigned index, unsigned id )
{
    switch (device)
    {
        case RETRO_DEVICE_KEYBOARD:
        {
            // godot::UtilityFunctions::print("Keyboard - Input state: port: ", port, " device: ", device,
            //                                " index: ", index, " id: ", id, LogLevel::DEBUG);
            
            if (id > RETROK_LAST) // Avoid out-of-bounds array access
            {
                godot::UtilityFunctions::print("Invalid key id: ", id);
                return 0;
            }

            auto st = keyboard_state[id];
            godot::UtilityFunctions::print("Input state: ", st);
            return st;
        }
        case RETRO_DEVICE_JOYPAD:

            // godot::UtilityFunctions::print("JoyPad - Input state: port: ", port, " device: ", device,
            //     " index: ", index, " id: ", id, LogLevel::DEBUG);

            if (id > RETRO_DEVICE_ID_JOYPAD_R3) {
                godot::UtilityFunctions::print("Invalid joypad id: ", id);
                return 0;
            }
            return joypad_state[id] ? 1 : 0;
        default:
            // godot::UtilityFunctions::print("Device: ", device, " not found");
            return 0;
    }
}

void GDRetro::forwarded_input( const godot::Ref<godot::InputEvent> &event )
{
    // keyboard
    if (event->is_class("InputEventKey")) {
        auto key_event = godot::Object::cast_to<godot::InputEventKey>(event.ptr());

        uint16_t modifiers = (RETROKMOD_ALT & (key_event->is_alt_pressed() ? 0xFF : 0)) |
                            (RETROKMOD_CTRL & (key_event->is_ctrl_pressed() ? 0xFF : 0)) |
                            (RETROKMOD_META & (key_event->is_meta_pressed() ? 0xFF : 0)) |
                            (RETROKMOD_SHIFT & (key_event->is_shift_pressed() ? 0xFF : 0));

        auto retro_key = godotKeyToRetroKey(key_event->get_key_label());

        if (retro_key <= RETROK_LAST) {
            keyboard_state[retro_key] = key_event->is_pressed();
        }

        // if (g_retro.retro_keyboard_event_callback) {
        //     g_retro.retro_keyboard_event_callback(key_event->is_pressed(), retro_key, 0, modifiers);
        // } else {
            godot::UtilityFunctions::printerr("[GDRetro] retro_keyboard_event_callback is not set.");
        // }
    }

    // joystick buttons
    if (event->is_class("InputEventJoypadButton")) {
        auto joy_event = godot::Object::cast_to<godot::InputEventJoypadButton>(event.ptr());
        int button_id = joy_event->get_button_index();

        auto retro_button_id = godotButtonToRetroKey(joy_event->get_button_index());


        if (button_id <= RETRO_DEVICE_ID_JOYPAD_R3) {
            joypad_state[retro_button_id] = joy_event->is_pressed();
        }

        // godot::UtilityFunctions::print("[GDRetro] Joypad Button ", button_id, " Pressed: ", joy_event->is_pressed());
    }

    // joystick analogic
    if (event->is_class("InputEventJoypadMotion")) {
        auto motion_event = godot::Object::cast_to<godot::InputEventJoypadMotion>(event.ptr());

        int axis = motion_event->get_axis();
        float value = motion_event->get_axis_value();

        // godot::UtilityFunctions::print("[GDRetro] Joypad Axis ", axis, " Value: ", value);
    }
}