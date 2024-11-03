#ifndef log_h
#define log_h

// Write a formatted log message to stdout.
void slog(const char* msg, ...);

// Write a formatted log message to stdout, clearing the previous line in stdout.
void slogc(const char* msg, ...);

#endif