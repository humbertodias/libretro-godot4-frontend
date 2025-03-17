#include "log.h"


static void core_log_base(enum retro_log_level level, const char *fmt, ...)
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

void core_log(enum retro_log_level level, const char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    char log_message[1024];
    vsnprintf(log_message, sizeof(log_message), fmt, args);

    godot::String msg = "[gdretro] " + godot::String(log_message);
    if(level == retro_log_level::RETRO_LOG_ERROR)
        godot::UtilityFunctions::printerr(msg);
    else
        godot::UtilityFunctions::print(msg);
  
    va_end(args);
}
