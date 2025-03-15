#include "gdretro.h"
#include "libretro.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/audio_stream_generator.hpp"
#include "godot_cpp/classes/audio_stream_generator_playback.hpp"
#include "godot_cpp/classes/audio_stream_player.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/window.hpp"

using namespace godot;

// Global variables
Ref<AudioStreamGenerator> audio_stream;
AudioStreamGeneratorPlayback *audio_playback = nullptr;

void GDRetro::core_audio_init(retro_system_av_info av)
{
    godot::UtilityFunctions::print("[GDRetro] Audio init");
    int sample_rate = static_cast<int>(av.timing.sample_rate);

    // Criar um AudioStreamGenerator
    audio_stream.instantiate();
    audio_stream->set_mix_rate(sample_rate);
    audio_stream->set_buffer_length(0.1f);

    // Criar um AudioStreamPlayer
    audio_player = memnew(AudioStreamPlayer);
    audio_player->set_stream(audio_stream);
    
    // Adicionar o AudioStreamPlayer à cena antes de chamar play()
    SceneTree *scene_tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    if (scene_tree)
    {
        Node *root = Object::cast_to<Node>(scene_tree->get_root());
        if (root)
        {
            root->add_child(audio_player);
        }
    }

    audio_player->play(); // Start playback

    Ref<AudioStreamPlayback> playback_ref = audio_player->get_stream_playback();
    if (playback_ref.is_valid())
    {
        audio_playback = Object::cast_to<AudioStreamGeneratorPlayback>(playback_ref.ptr());
    }

    if (!audio_playback)
    {
        godot::UtilityFunctions::printerr("[GDRetro] Error: Failed to cast playback to AudioStreamGeneratorPlayback.");
    }

}

void GDRetro::core_audio_sample(int16_t left, int16_t right)
{
    if (!audio_playback)
        return;

    godot::UtilityFunctions::print("[GDRetro] core_audio_sample: ", left, ", ", right);

    float left_f = left / 32768.0f;
    float right_f = right / 32768.0f;

    audio_playback->push_frame(Vector2(left_f, right_f));
}


size_t GDRetro::core_audio_sample_batch(const int16_t *data, size_t frames)
{

    if (!audio_playback)
        return 0;

    // Check if data is null or if the number of frames is zero
    if (!data || frames == 0)
        return 0;

    for (size_t i = 0; i < frames; ++i)
    {
        float left_f = data[i * 2] / 32768.0f;
        float right_f = data[i * 2 + 1] / 32768.0f;
        audio_playback->push_frame(Vector2(left_f, right_f));
    }

    return frames;
}
