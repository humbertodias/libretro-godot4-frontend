#include <godot_cpp/core/class_db.hpp>
#include "libretro.h"
#include "loader.h"
#include "gdretro.h"

using namespace godot;

GDRetro::GDRetro()
{
  godot::UtilityFunctions::print("[gdretro] Constructor called. Initializing singleton.");
  singleton = this;
  this->frame_buffer.unref(); // Reset the Ref<godot::Image> properly
  this->vfs.init_vfs_interface();
  godot::UtilityFunctions::print("[gdretro] Initialization complete.");
}

GDRetro::~GDRetro()
{
  godot::UtilityFunctions::print("[gdretro] Destructor called. Cleaning up resources.");
  this->_core_unload();
  singleton = nullptr;
  godot::UtilityFunctions::print("[gdretro] Resources cleaned up and singleton destroyed.");
}

GDRetro *GDRetro::singleton = nullptr;
GDRetro *GDRetro::get_singleton()
{
  return singleton;
}

void GDRetro::_ready()
{
  // Initialize the audio player using the constructor
  audio_player = memnew(AudioStreamPlayer); // Use memnew to allocate memory for AudioStreamPlayer
  add_child(audio_player);                  // Add it as a child to the scene

  // Set volume and start playing
  audio_player->set_volume_db(0.0);
}

void GDRetro::_process(double delta)
{
}

bool GDRetro::_core_load(String sofile)
{
  return core_load(sofile.utf8().get_data());
}

bool GDRetro::_core_load_game(String filename)
{
  return core_load_game(filename.utf8().get_data());
}

void GDRetro::_core_unload()
{
  core_unload();
}

Variant GDRetro::_core_get_system_info()
{
  if (!is_initialized())
  {
    return Variant();
  }

  struct retro_system_info system;
  core_get_system_info(&system);
  // struct retro_system_info system = core_get_system_info();

  Dictionary system_dict = Dictionary();
  system_dict["library_name"] = system.library_name;
  system_dict["library_version"] = system.library_version;
  system_dict["need_fullpath"] = system.need_fullpath;
  system_dict["block_extract"] = system.block_extract;

  return system_dict;
}

Variant GDRetro::_core_get_system_av_info()
{
  if (!is_initialized())
  {
    return Variant();
  }

  struct retro_system_av_info av;
  core_get_system_av_info(&av);
  // struct retro_system_av_info av = core_get_system_av_info();

  Dictionary av_geometry_dict = Dictionary();
  av_geometry_dict["aspect_ratio"] = av.geometry.aspect_ratio;
  av_geometry_dict["base_height"] = av.geometry.base_height;
  av_geometry_dict["base_width"] = av.geometry.base_width;
  av_geometry_dict["max_height"] = av.geometry.max_height;
  av_geometry_dict["max_width"] = av.geometry.max_width;

  Dictionary av_timing_dict = Dictionary();
  av_timing_dict["fps"] = av.timing.fps;
  av_timing_dict["sample_rate"] = av.timing.sample_rate;

  Dictionary av_dict = Dictionary();
  av_dict["geometry"] = av_geometry_dict;
  av_dict["timing"] = av_timing_dict;

  return av_dict;
}

void GDRetro::_core_run()
{
  core_run();
}

bool GDRetro::_is_initialized()
{
  return is_initialized();
}

void GDRetro::set_texture(const Ref<Texture2D> &p_texture)
{
  if (p_texture == texture)
  {
    return;
  }

  texture = p_texture;
  queue_redraw();
}

Ref<Texture2D> GDRetro::get_texture() const
{
  return texture;
}

void GDRetro::_get_rects(Rect2 &r_src_rect, Rect2 &r_dst_rect, bool &r_filter_clip_enabled) const
{
  Rect2 base_rect;

  r_filter_clip_enabled = false;
  base_rect = Rect2(0, 0, texture->get_width(), texture->get_height());

  Size2 frame_size = base_rect.size;

  r_src_rect.size = frame_size;
  r_src_rect.position = base_rect.position;

  Point2 dest_offset = offset;
  if (centered)
  {
    dest_offset -= frame_size / 2;
  }

  r_dst_rect = Rect2(dest_offset, frame_size);

  if (hflip)
  {
    r_dst_rect.size.x = -r_dst_rect.size.x;
  }
  if (vflip)
  {
    r_dst_rect.size.y = -r_dst_rect.size.y;
  }
}

void GDRetro::_notification(int p_what)
{
  switch (p_what)
  {
  case NOTIFICATION_DRAW:
  {
    if (texture.is_null())
    {
      return;
    }

    RID ci = get_canvas_item();

    Rect2 src_rect, dst_rect;
    bool filter_clip_enabled;
    _get_rects(src_rect, dst_rect, filter_clip_enabled);

    texture->draw_rect_region(ci, dst_rect, src_rect, Color(1, 1, 1), false, filter_clip_enabled);
  }
  break;
  }
}

void GDRetro::_bind_methods()
{
  ClassDB::bind_method(D_METHOD("core_load", "sofile"), &GDRetro::_core_load);
  ClassDB::bind_method(D_METHOD("core_load_game", "filename"), &GDRetro::_core_load_game);
  ClassDB::bind_method(D_METHOD("core_unload"), &GDRetro::_core_unload);
  ClassDB::bind_method(D_METHOD("core_get_system_info"), &GDRetro::_core_get_system_info);
  ClassDB::bind_method(D_METHOD("core_get_system_av_info"), &GDRetro::_core_get_system_av_info);
  ClassDB::bind_method(D_METHOD("core_run"), &GDRetro::_core_run);
  ClassDB::bind_method(D_METHOD("is_initialized"), &GDRetro::_is_initialized);

  ClassDB::bind_method(D_METHOD("set_texture", "texture"), &GDRetro::set_texture);
  ClassDB::bind_method(D_METHOD("get_texture"), &GDRetro::get_texture);
  ClassDB::bind_method(D_METHOD("get_frame_buffer"), &GDRetro::get_frame_buffer);
  ClassDB::bind_method(D_METHOD("forward_input", "event"), &GDRetro::forwarded_input);

  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture", "get_texture");
}