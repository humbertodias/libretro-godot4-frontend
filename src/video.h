#pragma once

#include "libretro.h"

static struct {
	// GLuint tex_id;
    // GLuint fbo_id;
    // GLuint rbo_id;

    // int glmajor;
    // int glminor;


	// GLuint pitch;
	// GLint tex_w, tex_h;
	// GLuint clip_w, clip_h;

	// GLuint pixfmt;
	// GLuint pixtype;
	// GLuint bpp;

    struct retro_hw_render_callback hw;
} g_video = {RETRO_HW_CONTEXT_NONE};