#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>

// Log structure to encapsulate logging functionality
typedef struct {
    FILE* file; // File pointer for logging
} Log;

/**
 * @brief Initializes the log with the given filename.
 * 
 * @param log Pointer to the Log structure to initialize.
 * @param filename Name of the log file.
 */
void initLog(Log* log, const char* filename);

/**
 * @brief Writes a message to the log file.
 * 
 * @param log Pointer to the Log structure.
 * @param message The message to be written to the log file.
 */
void writeToLog(Log* log, const char* message);

/**
 * @brief Logs a debug message to the log file.
 * 
 * @param log Pointer to the Log structure.
 * @param format Format string for the debug message.
 * @param ... Additional arguments for the format string.
 */
void logDebug(Log* log, const char* format, ...);

/**
 * @brief Logs an error message to the log file.
 * 
 * @param log Pointer to the Log structure.
 * @param line Line number where the error occurred.
 * @param format Format string for the error message.
 * @param ... Additional arguments for the format string.
 */
void logError(Log* log, int line, const char* format, ...);

/**
 * @brief Closes the log file.
 * 
 * @param log Pointer to the Log structure.
 */
void closeLog(Log* log);

#endif /* LOGGER_H */
