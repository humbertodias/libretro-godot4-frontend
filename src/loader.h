#pragma once

#include <libretro.h>

bool core_load(const char *sofile);
bool core_load_game(const char *filename);
void core_unload();
void core_get_system_info(struct retro_system_info *system);
void core_get_system_av_info(struct retro_system_av_info *av);
void core_run();
bool is_initialized();
// void audio_init(int frequency);  

static struct {
    void * handle;
    bool initialized;
  
    void (*retro_init)(void);
    void (*retro_deinit)(void);
    unsigned (*retro_api_version)(void);
    void (*retro_get_system_info)(struct retro_system_info* info);
    void (*retro_get_system_av_info)(struct retro_system_av_info* info);
    void (*retro_set_controller_port_device)(unsigned port, unsigned device);
    void (*retro_reset)(void);
    void (*retro_run)(void);
    // size_t retro_serialize_size(void);
    // bool retro_serialize(void *data, size_t size);
    // bool retro_unserialize(const void *data, size_t size);
    // void retro_cheat_reset(void);
    // void retro_cheat_set(unsigned index, bool enabled, const char *code);
    bool (*retro_load_game)(const struct retro_game_info* game);
    // bool retro_load_game_special(
    //   unsigned game_type,
    //   const struct retro_game_info *info,
    //   size_t num_info);
    void (*retro_unload_game)(void);
    // unsigned retro_get_region(void);
    // void *retro_get_memory_data(unsigned id);
    // size_t retro_get_memory_size(unsigned id);
    retro_keyboard_event_t retro_keyboard_event_callback;
  } g_retro;