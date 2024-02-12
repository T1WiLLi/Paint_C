#include <string.h>
#include <ctype.h>
#include "color.h"

/**
 * @brief Gets the red component of a color.
 * 
 * @param color Pointer to the Color instance.
 * @return The red component value.
 */
int getColorRed(const Color *color) {
    return color -> r;
}

/**
 * @brief Gets the green component of a color.
 * 
 * @param color Pointer to the Color instance.
 * @return The green component value.
 */
int getColorGreen(const Color *color) {
    return color -> g;
}

/**
 * @brief Gets the blue component of a color.
 * 
 * @param color Pointer to the Color instance.
 * @return The blue component value.
 */
int getColorBlue(const Color *color) {
    return color -> b;
}

/**
 * @brief Sets the color data file for a ColorTable instance.
 * 
 * @param inst Pointer to the ColorTable instance.
 * @param filepath Path to the color data file.
 * @param log Logger instance for error handling.
 */
void setColorDataFile(struct ColorTable * inst, char* filepath, Log log) {
    inst -> colorDataFile = fopen(filepath, "r");
    if (inst -> colorDataFile == NULL) {
        logError(&log, 45, "File is not reading");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Gets the color data file from a ColorTable instance.
 * 
 * @param inst Pointer to the ColorTable instance.
 * @return File pointer to the color data file.
 */
FILE* getColorDataFile(struct ColorTable * inst) {
    return inst -> colorDataFile;
}

char* trim(char *str) { // My Own implementation of the TRIM() function
    while(isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    char *end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    return str;
}

/**
 * @brief Loads color data from a CSV file into a ColorTable instance.
 * 
 * @param colorTable Pointer to the ColorTable instance.
 * @param log Logger instance for error handling.
 */
void loadColorTableFromCSV(struct ColorTable* colorTable, Log log) {
    if (colorTable -> colorDataFile == NULL) {
        logError(&log, 79, "Failed to open the file.");
        return;
    }

    char line[256];

    while(fgets(line, sizeof(line), colorTable -> colorDataFile)) {
        Color color;
        char* token = strtok(line, ",");
        if (token == NULL) continue;
            color.r = atoi(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
            color.g = atoi(token);
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
            color.b = atoi(token);
        
        token = strtok(NULL, ",");
        if (token == NULL) continue;
            color.name = strdup(trim(token));

        colorTable -> colorMappings = realloc(colorTable->colorMappings, sizeof(Color) * (colorTable -> colorCount + 1));
        if (colorTable -> colorMappings == NULL) {
            logError(&log, 105, "Memory Allocation Error");
            exit(EXIT_FAILURE);
        }
        colorTable -> colorMappings[colorTable -> colorCount++] = color;
    }
    fclose(colorTable -> getColorDataFile(colorTable));
}   

/**
 * @brief Constructor function to create a ColorTable instance.
 * 
 * @param log Logger instance for error handling.
 * @return Pointer to the newly created ColorTable instance.
 */
ColorTable* colorTableConstructor(Log log) {
    ColorTable* colorTable = malloc(sizeof(ColorTable));
    if (colorTable == NULL) {
        logError(&log, 121, "Memory Allocation Error");
        exit(EXIT_FAILURE);
    }

    colorTable -> setColorDataFile = &setColorDataFile;
    colorTable -> getColorDataFile = &getColorDataFile;

    colorTable -> colorMappings = NULL;
    colorTable -> colorCount = 0;
    return colorTable;
}

/**
 * @brief Destructor function to release resources used by a ColorTable instance.
 * 
 * @param colorTable Pointer to the ColorTable instance to be destroyed.
 */
void colorTableDeconstructor(ColorTable* colorTable) {
    if (colorTable != NULL) {
        free(colorTable -> colorMappings);
        free(colorTable);
    }
}
