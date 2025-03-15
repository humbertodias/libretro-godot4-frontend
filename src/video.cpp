#include "gdretro.h"
#include "libretro.h"
#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/image_texture.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

static float g_scale = 3;

static void resize_to_aspect(double ratio, int sw, int sh, int *dw, int *dh) {
	*dw = sw;
	*dh = sh;

	if (ratio <= 0)
		ratio = (double)sw / sh;

	if ((float)sw / sh < 1)
		*dw = *dh * ratio;
	else
		*dh = *dw / ratio;
}

void GDRetro::video_configure(const struct retro_game_geometry *geometry)
{

	int nwidth, nheight;

	resize_to_aspect(geometry->aspect_ratio, geometry->base_width * 1, geometry->base_height * 1, &nwidth, &nheight);

	nwidth *= g_scale;
	nheight *= g_scale;

    godot::UtilityFunctions::print("[GDRetro] Video init ", nwidth, " x ", nheight, " aspect ratio ", geometry->aspect_ratio);
    this->frame_buffer = godot::Image::create(nwidth, nheight, false, this->pixel_format);
}

void GDRetro::core_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
{
    if (!data || frame_buffer.is_null() || !frame_buffer.is_valid())
    {
        return;
    }

    if ((unsigned)frame_buffer->get_width() != width ||
        (unsigned)frame_buffer->get_height() != height)
    {
        godot::UtilityFunctions::print("[GDRetro] Resizing frame buffer to ", width, "x", height);
        auto created_frame_buffer = godot::Image::create(width, height, false, frame_buffer->get_format());
        if (created_frame_buffer.is_null() || !created_frame_buffer.is_valid())
        {
            godot::UtilityFunctions::printerr("[GDRetro] Failed to recreate frame buffer");
            return;
        }
        frame_buffer = created_frame_buffer;
    }

    unsigned bytes_per_pixel = 0;
    switch (frame_buffer->get_format())
    {
        case godot::Image::FORMAT_RGB565:
            bytes_per_pixel = 2;
            break;
        case godot::Image::FORMAT_RGBA8:
            bytes_per_pixel = 4;
            break;
        default:
            godot::UtilityFunctions::printerr("[GDRetro] Unhandled pixel format: ",
                                              frame_buffer->get_format());
            return;
    }

    unsigned row_size = width * bytes_per_pixel;
    godot::PackedByteArray intermediary_buffer;
    intermediary_buffer.resize(row_size * height);

    const uint8_t *src = static_cast<const uint8_t *>(data);
    uint8_t *dst = intermediary_buffer.ptrw();

    for (unsigned y = 0; y < height; y++)
    {
        memcpy(dst + y * row_size, src + y * pitch, row_size);
    }

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