#pragma once

#include "libretro.h"
#include <stdarg.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "godot_cpp/variant/utility_functions.hpp"

void core_log(enum retro_log_level level, const char *fmt, ...);
void gd_log(enum retro_log_level level, const char *fmt, ...);
void log(enum retro_log_level level, const char *fmt, ...);
void die(const char *fmt, ...);