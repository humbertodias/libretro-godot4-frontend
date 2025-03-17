#include "gdretro.h"
#include "loader.h"
#include "libretro.h"
#include "video.h"
#include "core_environment.h"
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
    case RETRO_ENVIRONMENT_SET_VARIABLES: {
        const struct retro_variable *vars = (const struct retro_variable *)data;
        size_t num_vars = 0;

        for (const struct retro_variable *v = vars; v->key; ++v) {
            num_vars++;
        }

        g_vars = (struct retro_variable*)calloc(num_vars + 1, sizeof(*g_vars));
        for (unsigned i = 0; i < num_vars; ++i) {
            const struct retro_variable *invar = &vars[i];
            struct retro_variable *outvar = &g_vars[i];

            const char *semicolon = strchr(invar->value, ';');
            const char *first_pipe = strchr(invar->value, '|');

            // SDL_assert(semicolon && *semicolon);
            semicolon++;
            while (isspace(*semicolon))
                semicolon++;

            if (first_pipe) {
                // outvar->value = malloc((first_pipe - semicolon) + 1);
                outvar->value = (const char*)malloc((first_pipe - semicolon) + 1);
                memcpy((char*)outvar->value, semicolon, first_pipe - semicolon);
                ((char*)outvar->value)[first_pipe - semicolon] = '\0';
            } else {
                outvar->value = strdup(semicolon);
            }

            outvar->key = strdup(invar->key);
            core_log(RETRO_LOG_DEBUG, "[gdretro] Variable ", invar->key, " = ", outvar->value);
            // SDL_assert(outvar->key && outvar->value);
        }

        return true;
    }
    case RETRO_ENVIRONMENT_GET_VARIABLE: {
        struct retro_variable *var = (struct retro_variable *)data;

        if (!g_vars)
            return false;

        for (const struct retro_variable *v = g_vars; v->key; ++v) {
            if (strcmp(var->key, v->key) == 0) {
                var->value = v->value;
                break;
            }
        }

        return true;
    }
    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE: {
        bool *bval = (bool*)data;
        *bval = false;
        return true;
    }
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
    case RETRO_ENVIRONMENT_SET_HW_RENDER: {
        struct retro_hw_render_callback *hw = (struct retro_hw_render_callback*)data;
        // TODO: implement
        // hw->get_current_framebuffer = core_get_current_framebuffer;
        // hw->get_proc_address = (retro_hw_get_proc_address_t)SDL_GL_GetProcAddress;
        g_video.hw = *hw;
        return true;
    }
    
    case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK: {
        // const struct retro_keyboard_callback
        auto callback = (const struct retro_keyboard_callback *)data;
        g_retro.retro_keyboard_event_callback = callback->callback;
        godot::UtilityFunctions::print( "[gdretro] keyboard callback set" );
    }
    break;


    case RETRO_ENVIRONMENT_GET_VFS_INTERFACE: {
        auto vfs_interface = (struct retro_vfs_interface_info *)data;
        godot::UtilityFunctions::print( "[gdretro] Core requested VFS interface" );
        if(vfs_interface->required_interface_version > this->vfs.supported_interface_version) {
            godot::UtilityFunctions::printerr( "[gdretro] Core requested VFS interface v", vfs_interface->required_interface_version, " we only support up to v", this->vfs.supported_interface_version );
            return false;
        }
        godot::UtilityFunctions::print( "[gdretro] Core requested VFS interface v", vfs_interface->required_interface_version);
        vfs_interface->iface = &this->vfs.vfs_interface;
        return true;
    }
    break;


    case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION: {
        unsigned *version = (unsigned *)data;
        // TODO: Support higher versions
        *version = 0;
        return false;
    }

    case RETRO_ENVIRONMENT_GET_MESSAGE_INTERFACE_VERSION: {
        unsigned *version = (unsigned *)data;
        *version = 0;
        return false;
    }
    break;    

    case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK: {
        struct retro_audio_callback *audio_cb = (struct retro_audio_callback*)data;
        audio_callback = *audio_cb;
        return true;
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

    case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME: {
        g_retro.supports_no_game = *(bool*)data;
        return true;
    }
    case RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE: {
        int *value = (int*)data;
        *value = 1 << 0 | 1 << 1;
        return true;
    }

    default:
    {
        core_log(RETRO_LOG_DEBUG, "[gdretro] Unhandled env #%u", cmd);
        return false;
    }
    }

    return true;
}