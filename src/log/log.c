#include <stdio.h>
#include <stdarg.h>

#include "log.h"

void slog(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");
}

void slogc(const char* msg, ...) {
    // clears previous line in stdout
    printf("\033[2K\r");

    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    fflush(stdout);
}