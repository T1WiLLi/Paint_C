#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

/**
 * @brief Structure representing a color.
 */
typedef struct Color {
    int r; /**< Red component of the color. */
    int g; /**< Green component of the color. */
    int b; /**< Blue component of the color. */
    const char* name; /**< Name of the color. */

    int(*getColorRed)(struct Color *); // Red getter
    int(*getColorGreen)(struct Color *); // Green getter
    int(*getColorBlue)(struct Color *); // Blue getter
} Color;

/**
 * @brief Structure representing a color table to manage colors.
 */
typedef struct ColorTable { 
    Color* colorMappings; // Dynamic array of Colors
    int colorCount;
    FILE* colorDataFile;

    void(*setColorDataFile)(struct ColorTable *, char* filepath, Log log);
    FILE*(*getColorDataFile)(struct ColorTable *);
} ColorTable;

/**
 * @brief Constructor function to create a ColorTable instance.
 * 
 * @param log Logger instance for error handling.
 * @return Pointer to the newly created ColorTable instance.
 */
ColorTable* colorTableConstructor(Log log);

/**
 * @brief Destructor function to release resources used by a ColorTable instance.
 * 
 * @param colorTable Pointer to the ColorTable instance to be destroyed.
 */
void colorTableDeconstructor(ColorTable* colorTable);

/**
 * @brief Loads color data from a CSV file into a ColorTable instance.
 * 
 * @param colorTable Pointer to the ColorTable instance.
 * @param log Logger instance for error handling.
 */
void loadColorTableFromCSV(struct ColorTable* colorTable, Log log);

/**
 * @brief Trim a given String to remove any whitespaces
 * 
 * @param str Pointer to the given String
*/
char* trim(char *str);

#endif /* COLOR_H */
