#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include "libretro.h"

using namespace godot;

/*class GDRetro : public CanvasItem {
  GDCLASS(GDRetro, CanvasItem);*/
class GDRetro : public Node2D
{
  GDCLASS(GDRetro, Node2D);

  bool centered = true;
  Point2 offset;
  bool hflip = false;
  bool vflip = false;

  AudioStreamPlayer *audio_player; // Using AudioStreamPlayer

  Ref<Texture2D> texture;
  void _texture_changed();
  void _get_rects(Rect2 &r_src_rect, Rect2 &r_dst_rect, bool &r_filter_clip_enabled) const;

private:
  static GDRetro *singleton;

protected:
  void _notification(int p_what);
  static void _bind_methods();

public:
  static GDRetro *get_singleton();

  GDRetro();
  ~GDRetro();
  void _ready() override;
  void _process(double delta) override;

  // loader
  bool _core_load(String sofile);
  bool _core_load_game(String filename);
  void _core_unload();
  Variant _core_get_system_info();
  Variant _core_get_system_av_info();
  void _core_run();
  bool _is_initialized();

  // image
  void set_texture(const Ref<Texture2D> &p_texture);
  Ref<Texture2D> get_texture() const;

  godot::Image::Format pixel_format;
  godot::Ref<godot::Image> frame_buffer;
  godot::Ref<godot::Image> get_frame_buffer()
  {
    return frame_buffer;
  }

  // video
  void video_init(const struct retro_game_geometry *geometry);
  void video_deinit();
  void core_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch);
  bool core_video_set_pixel_format(unsigned format);

  // environment
  bool core_environment(unsigned cmd, void *data);

  // input
  void core_input_poll(void);
  int16_t core_input_state(unsigned port, unsigned device, unsigned index, unsigned id);
  void forwarded_input(const godot::Ref<godot::InputEvent> &event);

  // audio
  void audio_init(double sample_rate);
  void audio_deinit();
  void core_audio_sample(int16_t left, int16_t right);
  size_t core_audio_sample_batch(const int16_t *data, size_t frames);
};
