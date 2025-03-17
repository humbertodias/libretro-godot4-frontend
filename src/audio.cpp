#include "gdretro.h"
#include "libretro.h"
#include "audio.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/audio_stream_generator.hpp"
#include "godot_cpp/classes/audio_stream_generator_playback.hpp"
#include "godot_cpp/classes/audio_stream_player.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/window.hpp"
#include "log.h"

using namespace godot;

// Global variables
Ref<AudioStreamGenerator> audio_stream;
AudioStreamGeneratorPlayback *audio_playback = nullptr;

void GDRetro::audio_init(double sample_rate)
{
    log(RETRO_LOG_DEBUG, "Audio init");
    audio_stream.instantiate();
    audio_stream->set_mix_rate(sample_rate);
    audio_stream->set_buffer_length(0.1f);

    audio_player = memnew(AudioStreamPlayer);
    audio_player->set_stream(audio_stream);
    
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
        log(RETRO_LOG_ERROR, "Error: Failed to cast playback to AudioStreamGeneratorPlayback.");
    }

    // Let the core know that the audio device has been initialized.
    if (audio_callback.set_state) {
        audio_callback.set_state(true);
    }

}

void GDRetro::audio_deinit(){

    log(RETRO_LOG_DEBUG, "Audio deinitializing");


    if (audio_callback.set_state) {
        audio_callback.set_state(false);
    }

    log(RETRO_LOG_DEBUG, "Audio deinitialized");
}

void GDRetro::core_audio_sample(int16_t left, int16_t right)
{
    if (!audio_playback)
        return;

    log(RETRO_LOG_DEBUG, "core_audio_sample: ", left, ", ", right);

    float left_f = left / 32768.0f;
    float right_f = right / 32768.0f;

    audio_playback->push_frame(Vector2(left_f, right_f));
}


size_t GDRetro::core_audio_sample_batch(const int16_t *data, size_t frames)
{

    if (!audio_playback)
        return 0;

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
