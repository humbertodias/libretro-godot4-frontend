#include "gdretro.h"
#include "libretro.h"
#include <cstdarg> // Required for va_start and va_end

static void core_log(enum retro_log_level level, const char *fmt, ...)
{
    char buffer[4096] = {0};
    static const char *levelstr[] = {
        "dbg",
        "inf",
        "wrn",
        "err"};
    va_list va;

    va_start(va, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, va);
    va_end(va);

    if (level == 0)
        return;

    fprintf(stderr, "[%s] %s", levelstr[level], buffer);
    fflush(stderr);

    if (level == RETRO_LOG_ERROR)
    {
        exit(EXIT_FAILURE);
    }
}

bool GDRetro::core_environment(unsigned cmd, void *data)
{
    switch (cmd)
    {
    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
    {
        struct retro_log_callback *cb = (struct retro_log_callback *)data;
        cb->log = core_log;
    }
    break;

    case RETRO_ENVIRONMENT_GET_CAN_DUPE:
    {
        bool *bval = (bool *)data; // NOLINT
        *bval = true;
    }
    break;

    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
    {
        const enum retro_pixel_format *fmt = (enum retro_pixel_format *)data;

        if (*fmt > RETRO_PIXEL_FORMAT_RGB565)
        {
            return false;
        }

        core_log(RETRO_LOG_DEBUG, "[gdretro] Core setting pixel format");
        return GDRetro::get_singleton()->core_video_set_pixel_format(*fmt);
    }

    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
    case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY:
    case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
    {
        *(const char **)data = ".";
        return true;
    }

    case RETRO_ENVIRONMENT_SET_MESSAGE:
    {
        const struct retro_message *message = (const struct retro_message *)data;
        core_log(
            RETRO_LOG_DEBUG,
            "[gdretro] RETRO_ENVIRONMENT_SET_MESSAGE: %s\n",
            message->msg);
        break;
    }

    case RETRO_ENVIRONMENT_SHUTDOWN:
    {
        core_log(RETRO_LOG_DEBUG, "[gdretro] RETRO_ENVIRONMENT_SHUTDOWN");
        break;
    }

    default:
    {
        core_log(RETRO_LOG_DEBUG, "[gdretro] Unhandled env #%u", cmd);
        return false;
    }
    }

    return true;
}