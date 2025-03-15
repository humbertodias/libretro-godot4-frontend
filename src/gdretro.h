#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/image.hpp>

using namespace godot;

/*class GDRetro : public CanvasItem {
  GDCLASS(GDRetro, CanvasItem);*/
class GDRetro : public Node2D {
  GDCLASS(GDRetro, Node2D);
  
  bool centered = true;
  Point2 offset;
  bool hflip = false;
  bool vflip = false;
  
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
    bool _core_load(String sofile);
    bool _core_load_game(String filename);
    void _core_unload();
    Variant _core_get_system_info();
    Variant _core_get_system_av_info();
    void _core_run();
    bool _is_initialized();
    
    void set_texture(const Ref<Texture2D> &p_texture);
	  Ref<Texture2D> get_texture() const;

    void video_configure( const struct retro_game_geometry *geometry );
    void core_video_refresh( const void *data, unsigned width, unsigned height, size_t pitch );
    bool core_video_set_pixel_format( unsigned format );
    bool core_environment(unsigned cmd, void * data);

    godot::Image::Format pixel_format;
    godot::Ref<godot::Image> frame_buffer;
    godot::Ref<godot::Image> get_frame_buffer()
    {
        return frame_buffer;
    }

};
