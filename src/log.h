#pragma once

#include "libretro.h"
#include <stdarg.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "godot_cpp/variant/utility_functions.hpp"

static void core_log_base(enum retro_log_level level, const char *fmt, ...);
void core_log(enum retro_log_level level, const char *fmt, ...);
