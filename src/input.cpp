#include "gdretro.h"
#include "keyboard_map.h"
#include "godot_cpp/classes/input_event_key.hpp"

#include "godot_cpp/variant/utility_functions.hpp"
#include "loader.h"

static bool keyboard_state[RETROK_LAST] = { false };

void GDRetro::core_input_poll( void )
{
    // TODO
    // godot::UtilityFunctions::print( "Input poll" );
}

int16_t GDRetro::core_input_state( unsigned port, unsigned device, unsigned index, unsigned id )
{
    godot::UtilityFunctions::print( "[GDRetro] core_input_state: port: ", port, " device: ", device, " index: ", index, " id: ", id);
    switch (device)
    {
        case RETRO_DEVICE_KEYBOARD:
        {
            godot::UtilityFunctions::print("Input state: port: ", port, " device: ", device,
                                           " index: ", index, " id: ", id);
            
            if (id > RETROK_LAST) // Avoid out-of-bounds array access
            {
                godot::UtilityFunctions::print("Invalid key id: ", id);
                return 0;
            }

            auto st = keyboard_state[id];
            godot::UtilityFunctions::print("Input state: ", st);
            return st;
        }
        default:
            godot::UtilityFunctions::print("Device: ", device, " not found");
            return 0;
    }
}

void GDRetro::forwarded_input( const godot::Ref<godot::InputEvent> &event )
{
    godot::UtilityFunctions::print( "forwarded_input " ); 
    if ( event->is_class( "InputEventKey" ) )
    {
        auto key_event = godot::Object::cast_to<godot::InputEventKey>( event.ptr() );

        // Up the bool value 0x1 to 0xFF so we have enough bits for the & operation
        uint16_t modifiers = (RETROKMOD_ALT & ( key_event->is_alt_pressed() ? 0xFF : 0 )) |
                             (RETROKMOD_CTRL & ( key_event->is_ctrl_pressed() ? 0xFF : 0 )) |
                             (RETROKMOD_META & ( key_event->is_meta_pressed() ? 0xFF : 0 )) |
                             (RETROKMOD_SHIFT & ( key_event->is_shift_pressed() ? 0xFF : 0 ));

        auto retro_key = godotKeyToRetroKey(key_event->get_key_label());

        godot::UtilityFunctions::print( "Key event: echo: ", key_event->is_echo(),
                                " key label: ", key_event->get_key_label(),
                                " key code: ", key_event->get_keycode(),
                                " physical key: ", key_event->get_physical_keycode(),
                                " pressed: ", key_event->is_pressed(),
                                " retro key: ", retro_key );

        if(retro_key > RETROK_LAST)
            return;

        if (g_retro.retro_keyboard_event_callback) {
            g_retro.retro_keyboard_event_callback(key_event->is_pressed(), retro_key, 0, modifiers);
        } else {
            godot::UtilityFunctions::printerr("[GDRetro] retro_keyboard_event_callback is not set.");
        }

        keyboard_state[retro_key] = key_event->is_pressed();
    }
}