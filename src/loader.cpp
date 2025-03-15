/**
 * noarch: a small libretro frontend that doesn't provide any video, audio or input.
 *
 * It loads a Core, along with its Content, runs an iteration, and then quits. This is useful for unit testing.
 *
 * License: BSD 3-Clause "New" or "Revised" License
 *
 * Copyright 2021 Rob Loach (@RobLoach)
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <dlfcn.h>
#include "libretro.h"
#include "loader.h"

#include "gdretro.h"


#define load_sym(V, S) do {\
  if (!((*(void**)&V) = dlsym(g_retro.handle, #S))) /** NOLINT **/ \
    die("[gdretro] Failed to load symbol '" #S "'': %s", dlerror()); \
  } while (0)
#define load_retro_sym(S) load_sym(g_retro.S, S)

static void die(const char * fmt, ...) {
  char buffer[4096];

  va_list va;
  va_start(va, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, va);
  va_end(va);

  fputs(buffer, stderr);
  fputc('\n', stderr);
  fflush(stderr);

  exit(EXIT_FAILURE);
}

static void video_deinit() {}

static void audio_init(int frequency) {
  (void)frequency;
}

static void audio_deinit() {}


static void core_input_poll(void) {
  // Nothing
}

// static int16_t core_input_state(
//       unsigned port,
//       unsigned device,
//       unsigned index,
//       unsigned id) {
//   (void)port;
//   (void)device;
//   (void)index;
//   (void)id;
//   return 0;
// }

// static void core_audio_sample(int16_t left, int16_t right) {
//   (void)left;
//   (void)right;
// }

// static size_t core_audio_sample_batch(const int16_t * data, size_t frames) {
//   (void)data;
//   (void)frames;
//   return 0;
// }

bool core_load(const char * sofile) {
  void (*set_environment)(retro_environment_t) = NULL;
  void (*set_video_refresh)(retro_video_refresh_t) = NULL;
  void (*set_input_poll)(retro_input_poll_t) = NULL;
  void (*set_input_state)(retro_input_state_t) = NULL;
  void (*set_audio_sample)(retro_audio_sample_t) = NULL;
  void (*set_audio_sample_batch)(retro_audio_sample_batch_t) = NULL;

  memset( & g_retro, 0, sizeof(g_retro));
  g_retro.handle = dlopen(sofile, RTLD_LAZY);

  if (!g_retro.handle) {
    printf("[gdretro] Failed to load core: %s\n", dlerror());
    return false;
  }

  dlerror();

  load_retro_sym(retro_init);
  load_retro_sym(retro_deinit);
  load_retro_sym(retro_api_version);
  load_retro_sym(retro_get_system_info);
  load_retro_sym(retro_get_system_av_info);
  load_retro_sym(retro_set_controller_port_device);
  load_retro_sym(retro_reset);
  load_retro_sym(retro_run);
  load_retro_sym(retro_load_game);
  load_retro_sym(retro_unload_game);
  // load_retro_sym(retro_keyboard_event_callback);

  load_sym(set_environment, retro_set_environment);
  load_sym(set_video_refresh, retro_set_video_refresh);
  load_sym(set_input_poll, retro_set_input_poll);
  load_sym(set_input_state, retro_set_input_state);
  // load_sym(retro_keyboard_event_callback, retro_keyboard_event_callback);
  load_sym(set_audio_sample, retro_set_audio_sample);
  load_sym(set_audio_sample_batch, retro_set_audio_sample_batch);

 
  set_environment([]( unsigned cmd, void *data ) { 
    return GDRetro::get_singleton()->core_environment( cmd, data ); } 
  );

  set_video_refresh( []( const void *data, unsigned width, unsigned height, size_t pitch ) {

    GDRetro::get_singleton()->core_video_refresh( data, width, height, pitch );
  });

  set_input_poll( []( void ) { GDRetro::get_singleton()->core_input_poll(); } );

  set_input_state( []( unsigned port, unsigned device, unsigned index, unsigned id ) {
        return GDRetro::get_singleton()->core_input_state( port, device, index, id );
  } );

  set_audio_sample(
      []( int16_t left, int16_t right ) { GDRetro::get_singleton()->core_audio_sample( left, right ); } );

  set_audio_sample_batch( []( const int16_t *data, size_t frames ) {
      return GDRetro::get_singleton()->core_audio_sample_batch( data, frames );
  } );


  // set_input_poll(core_input_poll);
  // set_input_state(core_input_state);

  // set_audio_sample(core_audio_sample);
  // set_audio_sample_batch(core_audio_sample_batch);
  
  g_retro.retro_init();
  g_retro.initialized = true;
  
  puts("[gdretro] Core loaded");
  
  return true;
}

bool core_load_game(const char * filename) {
  struct retro_system_timing timing = {
    60.0f, 10000.0f
  };
  struct retro_game_geometry geom = {
    100, 100, 100, 100, 1.0f
  };
  struct retro_system_av_info av = {
    geom, timing
  };
  struct retro_system_info system = {
    0, 0, 0, false, false
  };

  struct retro_game_info info = {
    filename,
    0,
    0,
    NULL
  };

  FILE* file = NULL;
  if (filename) {
    file = fopen(filename, "rb");

    if (!file) {
      printf("[gdretro] Error: Failed to load content from '%s'\n", filename);
      return false;
    }

    fseek(file, 0, SEEK_END);
    info.size = ftell(file);
    rewind(file);
  }

  g_retro.retro_get_system_info(&system);
  printf("[gdretro] Info: %s %s\n", system.library_name, system.library_version);

  if (filename && !system.need_fullpath) {
    info.data = malloc(info.size);

    if (!info.data || !fread((void * ) info.data, info.size, 1, file)) { // NOLINT
      puts("[gdretro] Error: Failed to load game data.");
      fclose(file);
      return false;
    }
    fclose(file);
  }

  if (!g_retro.retro_load_game(&info)) {
    puts("[gdretro] The core failed to load the game.");
    return false;
  }

  g_retro.retro_get_system_av_info(&av);
  printf("[gdretro] Video: %ix%i\n",
    av.geometry.base_width,
    av.geometry.base_height);

  GDRetro::get_singleton()->video_configure( &av.geometry );

  audio_init(av.timing.sample_rate);
  return true;
}

void core_unload() {
  if (g_retro.initialized) {
    g_retro.retro_deinit();
  }

  if (g_retro.handle) {
    dlclose(g_retro.handle);
  }
  
  audio_deinit();
  video_deinit();
  
  puts("[gdretro] Core unloaded");
}

void core_get_system_info(struct retro_system_info *system) {
  g_retro.retro_get_system_info(system);
}

void core_get_system_av_info(struct retro_system_av_info *av) {
  g_retro.retro_get_system_av_info(av);
}

void core_run() {
  g_retro.retro_run();
}

bool is_initialized() {
  return g_retro.initialized;
}


/*int main(int argc, char * argv[]) {
  // Ensure proper amount of arguments.
  if (argc < 2) {
    printf("Usage: %s <core> [game]\n", argv[0]);
    return 1;
  }

  // Load the core.
  if (!core_load(argv[1])) {
    return 1;
  }
  puts("[gdretro] Core loaded");

  // Load the game if needed.
  if (!core_load_game((argc > 2) ? argv[2] : NULL)) {
    return 1;
  }

  // Run an iteration.
  puts("[gdretro] retro_run()");
  g_retro.retro_run();

  // Unload the core.
  core_unload();
  audio_deinit();
  video_deinit();
  puts("[gdretro] Core unloaded");

  return 0;
}*/
