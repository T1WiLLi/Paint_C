#include <windows.h>
#include <stdio.h> // For file handling
#include "howTo.h"

void ShowHowToDialog(HWND hwndParent) {
    FILE *file;
    char line[256];
    char dialogText[4096] = ""; // Buffer for dialog text

    // Open the text file containing the HOW-TO content
    file = fopen("./assets/howToContent.txt", "r");
    if (file == NULL) {
        MessageBox(hwndParent, "Failed to open HOW-TO content file.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        strcat(dialogText, line);
    }
    fclose(file);

    MessageBox(hwndParent, dialogText, "How To Use", MB_OK | MB_ICONINFORMATION);
}

