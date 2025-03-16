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

  struct VFS
    {
        uint32_t supported_interface_version = 0;

        // Called on RetroHost's constructor
        void init_vfs_interface();
        struct retro_vfs_interface vfs_interface;

        // /* VFS API v1 */
        /* Get path from opaque handle. Returns the exact same path passed to file_open when getting
         * the handle Introduced in VFS API v1 */
        const char *get_path( retro_vfs_file_handle *stream );
        /* Open a file for reading or writing. If path points to a directory, this will
         * fail. Returns the opaque file handle, or NULL for error.
         * Introduced in VFS API v1 */
        struct retro_vfs_file_handle *open( const char *path, unsigned mode, unsigned hints );
        /* Close the file and release its resources. Must be called if open_file returns non-NULL.
         * Returns 0 on success, -1 on failure. Whether the call succeeds ot not, the handle passed
         * as parameter becomes invalid and should no longer be used. Introduced in VFS API v1 */
        int close( retro_vfs_file_handle *stream );
        /* Return the size of the file in bytes, or -1 for error.
         * Introduced in VFS API v1 */
        int64_t size( struct retro_vfs_file_handle *stream );
        /* Get the current read / write position for the file. Returns -1 for error.
         * Introduced in VFS API v1 */
        int64_t tell( struct retro_vfs_file_handle *stream );
        /* Set the current read/write position for the file. Returns the new position, -1 for error.
         * Introduced in VFS API v1 */
        int64_t seek( struct retro_vfs_file_handle *stream, int64_t offset, int seek_position );
        /* Read data from a file. Returns the number of bytes read, or -1 for error.
         * Introduced in VFS API v1 */
        int64_t read( struct retro_vfs_file_handle *stream, void *s, uint64_t len );
        /* Write data to a file. Returns the number of bytes written, or -1 for error.
         * Introduced in VFS API v1 */
        int64_t write( struct retro_vfs_file_handle *stream, const void *s, uint64_t len );
        /* Flush pending writes to file, if using buffered IO. Returns 0 on sucess, or -1 on
         * failure. Introduced in VFS API v1 */
        int flush( struct retro_vfs_file_handle *stream );
        /* Delete the specified file. Returns 0 on success, -1 on failure
         * Introduced in VFS API v1 */
        int remove( const char *path );
        /* Rename the specified file. Returns 0 on success, -1 on failure
         * Introduced in VFS API v1 */
        int rename( const char *old_path, const char *new_path );
        // /* VFS API v2 */
        /* Truncate file to specified size. Returns 0 on success or -1 on error
         * Introduced in VFS API v2 */
        int64_t truncate( struct retro_vfs_file_handle *stream, int64_t length );
        // /* VFS API v3 */
        /* Stat the specified file. Retruns a bitmask of RETRO_VFS_STAT_* flags, none are set if
         * path was not valid. Additionally stores file size in given variable, unless NULL is
         * given. Introduced in VFS API v3 */
        int stat( const char *path, int32_t *size );
        /* Create the specified directory. Returns 0 on success, -1 on unknown failure, -2 if
         * already exists. Introduced in VFS API v3 */
        int mkdir( const char *dir );
        /* Open the specified directory for listing. Returns the opaque dir handle, or NULL for
         * error. Support for the include_hidden argument may vary depending on the platform.
         * Introduced in VFS API v3 */
        struct retro_vfs_dir_handle *opendir( const char *dir, bool include_hidden_files );
        /* Read the directory entry at the current position, and move the read pointer to the next
         * position. Returns true on success, false if already on the last entry. Introduced in VFS
         * API v3 */
        bool read_dir( struct retro_vfs_dir_handle *dir_stream );
        /* Get the name of the last entry read. Returns a string on success, or NULL for error.
         * The returned string pointer is valid until the next call to readdir or closedir.
         * Introduced in VFS API v3 */
        const char *dirent_get_name( struct retro_vfs_dir_handle *dir_stream );
        /* Check if the last entry read was a directory. Returns true if it was, false otherwise (or
         * on error). Introduced in VFS API v3 */
        bool dirent_is_dir( struct retro_vfs_dir_handle *dir_stream );
        /* Close the directory and release its resources. Must be called if opendir returns
         * non-NULL. Returns 0 on success, -1 on failure. Whether the call succeeds ot not, the
         * handle passed as parameter becomes invalid and should no longer be used. Introduced in
         * VFS API v3 */
        int closedir( struct retro_vfs_dir_handle *dir_stream );
    } vfs;
  // vfs
};
