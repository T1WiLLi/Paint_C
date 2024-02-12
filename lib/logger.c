#include "logger.h"

void initLog(Log* log, const char* filename) {
    log->file = fopen(filename, "a");
    if (log->file == NULL) {
        perror("Error opening log file");
    }
}

void writeToLog(Log* log, const char* message) {
    if (log->file != NULL) {
        fprintf(log->file, "%s\n", message);
        fflush(log->file);
    }
}

void logDebug(Log* log, const char* format, ...) {
    if (log->file != NULL) {
        va_list args;
        va_start(args, format);
        vfprintf(log->file, "DEBUG: ", args);
        vfprintf(log->file, format, args);
        va_end(args);
        fprintf(log->file, "\n");
        fflush(log->file);
    }
}

void logError(Log* log, int line, const char* format, ...) {
    if (log->file != NULL) {
        va_list args;
        va_start(args, format);
        fprintf(log->file, "ERROR at line %d: ", line);
        vfprintf(log->file, format, args);
        va_end(args);
        fprintf(log->file, "\n");
        fflush(log->file);
    }
}

void closeLog(Log* log) {
    if (log->file != NULL) {
        fclose(log->file);
    }
}
