#include "gdretro.h"
#include "libretro.h"
#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/image_texture.hpp"
#include "godot_cpp/variant/utility_functions.hpp"


void GDRetro::core_video_init(const struct retro_game_geometry *geometry)
{

    if (!frame_buffer.is_valid()) {
        godot::UtilityFunctions::printerr("[GDRetro] Framebuffer initialization failed.");
        // return;
    }

    godot::UtilityFunctions::print("[GDRetro] Video init ", geometry->base_width, " x ",
                                    geometry->base_height);
    this->frame_buffer = godot::Image::create(geometry->base_width, geometry->base_height, false,
                                              this->pixel_format);
}

void GDRetro::core_video_refresh(const void *data, unsigned width, unsigned height,
                                   size_t pitch)
{
    if (!data || frame_buffer.is_null() || !frame_buffer.is_valid())
    {
        return;
    }

    if ((unsigned)frame_buffer->get_width() != width ||
        (unsigned)frame_buffer->get_height() != height)
    {
        godot::UtilityFunctions::print("[GDRetro] Resizing frame buffer to ", width, "x",
                                       height);
        auto created_frame_buffer =
            godot::Image::create(width, height, false, frame_buffer->get_format());
        if (created_frame_buffer.is_null() || !created_frame_buffer.is_valid())
        {
            godot::UtilityFunctions::printerr("[GDRetro] Failed to recreate frame buffer");
            return;
        }
        frame_buffer = created_frame_buffer;
    }

    unsigned buffer_size;
    switch (frame_buffer->get_format())
    {
        case godot::Image::FORMAT_RGB565:
            buffer_size = width * height * 2;
            break;
        case godot::Image::FORMAT_RGBA8:
        {
            buffer_size = width * height * 4;

#if defined(_MSC_VER) // MSVC compiler
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

            uint32_t *data32 = (uint32_t *)data;
            for (unsigned i = 0; i < width * height; i++)
            {
                uint32_t pixel = data32[i];
                uint8_t alpha = (pixel & 0xFF000000) >> 24;
                uint8_t red = (pixel & 0x00FF0000) >> 16;
                uint8_t green = (pixel & 0x0000FF00) >> 8;
                uint8_t blue = (pixel & 0x000000FF);
                data32[i] = (alpha << 24) | (blue << 16) | (green << 8) | red;
            }

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
        }
        break;
        default:
            godot::UtilityFunctions::printerr("[GDRetro] Unhandled pixel format: ",
                                              frame_buffer->get_format());
            return;
    }

    godot::PackedByteArray intermediary_buffer;
    intermediary_buffer.resize(buffer_size);
    memcpy((void *)intermediary_buffer.ptr(), data, buffer_size);

    frame_buffer->set_data(width, height, false, frame_buffer->get_format(), intermediary_buffer);

    texture = ImageTexture::create_from_image(frame_buffer);
}

bool GDRetro::core_video_set_pixel_format(unsigned format)
{
    switch (format)
    {
        case RETRO_PIXEL_FORMAT_0RGB1555:
            godot::UtilityFunctions::print("[GDRetro] Pixel format: 0RGB1555");
            this->pixel_format = godot::Image::Format::FORMAT_RGB565;
            return true;
        case RETRO_PIXEL_FORMAT_XRGB8888:
            godot::UtilityFunctions::print("[GDRetro] Pixel format: XRGB8888");
            this->pixel_format = godot::Image::Format::FORMAT_RGBA8;
            return true;
        case RETRO_PIXEL_FORMAT_RGB565:
            godot::UtilityFunctions::print("[GDRetro] Pixel format: RGB565");
            this->pixel_format = godot::Image::Format::FORMAT_RGB565;
            return true;
        default:
            return false;
    }
}