/*
    Paint Program - Documentation 

    Author : William Beaudin
    Version : 3.2
    Last Modification : 2024-02-09
    Copyright : Free & Available

    Table of Contents:
        1- Libraries import
        2- #Defined
        3- Typedef of multiple structure and Objects
        4- Functions prototypes
        5- Main Windows and Process Functions
        6- Primary Functions

    Functionnality : 

        Paint-C is a pseudo recreation of the MS Paint from Windows 95
        In such, you have access to some *featured* modes such as 
        a line-mode, grid-mode and eraser. You can also use custom RGB
        color as well as 9 pre-defined color. The most advanced part lies
        in the saving-loading of you're drawing.
    
    Technology used : 

        Because we are working in Plain C, the technology used are 
        relativly simple. We use the Win32 API to generate windows, 
        frame and rectangular shape. We also use some specifically 
        designed libraries such as the Debuggerlib.c, color.c and Paintlib.c
        Those libraries are trademark of the Paint C projects.
        Altought the Debugger could be intersting for other projects.     
*/

// Windows API Libraries
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <wingdi.h>
#include <dbghelp.h>

// Standard C development Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Custom Libraries
#include "./lib/logger.h"
#include "./lib/color.h"
#include "./lib/howTo.h"

// Color ID
#define ID_COLOR_BLACK         101
#define ID_COLOR_RED           102
#define ID_COLOR_GREEN         103
#define ID_COLOR_BLUE          104
#define ID_COLOR_YELLOW        105
#define ID_COLOR_ORANGE        106
#define ID_COLOR_PURPLE        107
#define ID_COLOR_GRAY          108
#define ID_COLOR_BROWN         109

// Custom Color ID
#define ID_CUSTOM_COLOR_LABEL  201
#define ID_CUSTOM_RGB_VALUE    202
#define ID_CUSTOM_BUTTON_COLOR 203

// Mode Settings ID
#define ID_FREE_MODE           301
#define ID_GRID_MODE           302
#define ID_LINE_MODE           303
#define ID_TEXT_MODE           304
#define ID_ERASER_MODE         305

// Brush Settings
#define ID_BRUSH_SLIDER        401
#define ID_BRUSH_SQUARE_MODE   402
#define ID_BRUSH_CIRCLE_MODE   403
#define ID_BRUSH_MIN             1 //  1 is defined as reserved here
#define ID_BRUSH                26 // 26 is defined as reserved here

// Screen Settings 
#define SCREEN_WIDTH          1280 // 1280 is defined as reserved here 
#define SCREEN_HEIGHT          720 // 720 is defined as reserved here

// Miscellaneous
#define ID_RESET               501
#define ID_SAVE_BUTTON         502
#define ID_LOAD_BUTTON         503

// Progress Save-bar
#define ID_PROGRESS_DIALOG    1001
#define ID_PROGRESS_BAR       1002
#define ID_PROGRESS_TEXT      1003

// UI & Visuals (They are all defined as reserved!)
#define COLOR_BUTTON_WIDTH      20
#define COLOR_BUTTON_HEIGHT     20
#define COLOR_BUTTON_SPACING     5
#define COLOR_BUTTON_GRID_SIZE   3 // Number of squares per row and column
#define COLOR_GRID_OFFSET_X    275 // Offset to shift the grid horizontally
#define PUSH_BUTTON_WIDTH       80
#define PUSH_BUTTON_HEIGHT      30
#define BRUSH_SLIDER_WIDTH     200
#define BRUSH_SLIDER_HEIGHT     30
#define BRUSH_BUTTON_WIDTH     200
#define BRUSH_BUTTON_HEIGHT     30
#define TOOLBAR_HEIGHT          80



// Brush Object
typedef struct Brush {
    char* brushName;    // Name of the brush
    int mode;           // Mode of the brush: free(300), grid(301), line(302), eraser(303)
    int size;           // Size of the brush
    int drawMode;
    HBRUSH colorBrush;  // Brush color
    int currentColor[3]; // Current RGB color code
    int brushPos[2];    // Brush position: x and y coordinates

    // Function pointers for setting and getting brush attributes
    void (*setBrushName)(struct Brush *, char*);
    char* (*getBrushName)(struct Brush *);
    
    void (*setBrushMode)(struct Brush *, int);
    int (*getBrushMode)(struct Brush *);

    void (*setBrushSize)(struct Brush *, int);
    int (*getBrushSize)(struct Brush *);

    void (*setBrushDrawMode)(struct Brush *, int);
    int (*getBrushDrawMode)(struct Brush *);

    void (*setColorBrush)(struct Brush *, HBRUSH);
    HBRUSH (*getColorBrush)(struct Brush *);

    void (*setCurrentColor)(struct Brush *, int*);
    int* (*getCurrentColor)(struct Brush *);

    void (*setBrushPos)(struct Brush *, int*);
    int* (*getBrushPos)(struct Brush *);
} Brush;


/**
 * @brief Sets the name of the brush.
 * 
 * @param inst Pointer to the Brush instance.
 * @param name Name to set for the brush.
 */
void setBrushName(Brush * inst, char* name) {
    inst -> brushName = malloc(strlen(name) + 1);
    strcpy(inst -> brushName, name);
}

/**
 * @brief Gets the name of the brush.
 * 
 * @param inst Pointer to the Brush instance.
 * @return Name of the brush.
 */
char* getBrushName(Brush * inst) {
    return inst -> brushName;
}

/**
 * @brief Sets the mode of the brush.
 * 
 * @param inst Pointer to the Brush instance.
 * @param mode Mode to set for the brush.
 */
void setBrushMode(Brush * inst, int mode) {
    inst -> mode = mode;
}

/**
 * @brief Gets the mode of the brush.
 * 
 * @param inst Pointer to the Brush instance.
 * @return Mode of the brush.
 */
int getBrushMode(Brush * inst) {
    return inst -> mode;
}

/**
 * @brief Sets the size of the brush.
 * 
 * @param inst Pointer to the Brush instance.
 * @param size Size to set for the brush.
 */
void setBrushSize(Brush * inst, int size) {
    inst -> size = size;
}

/**
 * @brief Gets the size of the brush.
 * 
 * @param inst Pointer to the Brush instance.
 * @return Size of the brush.
 */
int getBrushSize(Brush * inst) {
    return inst -> size;
}

/**
 * @brief Sets the mode of the drawing brush (square or circle).
 * 
 * @param inst Pointer to the Brush instance.
 * @param drawMode Mode to set for the brush.
*/
void setBrushDrawMode(Brush * inst, int drawMode) {
    inst -> drawMode = drawMode;
}

/**
 * @brief Gets the drawMode of the brush.
 * 
 * @param inst Pointer to the Brush instance.
 * @return DrawingMode of the brush.
*/
int getBrushDrawMode(Brush * inst) {
    return inst -> drawMode;
}

/**
 * @brief Sets the color brush attribute of a Brush instance.
 * 
 * @param brush Pointer to the Brush instance.
 * @param colorBrush New color brush to be set.
 */
void setColorBrush(Brush * brush, HBRUSH colorBrush) {
    brush -> colorBrush = colorBrush;
}

/**
 * @brief Gets the color brush attribute of a Brush instance.
 * 
 * @param brush Pointer to the Brush instance.
 * @return HBRUSH representing the color brush.
 */
HBRUSH getColorBrush(Brush * brush) {
    return brush -> colorBrush;
}

/**
 * @brief Sets the current color of a brush.
 * 
 * @param brush Pointer to the Brush struct whose current color will be set.
 * @param currentColor Pointer to an array containing the color components (RGB values).
 *        It should be an array of size 3 containing the red, green, and blue components, respectively.
 */
void setCurrentColor(Brush *brush, int* currentColor) {
    if (brush == NULL || currentColor == NULL) {
        return;
    }
    
    for (int i = 0; i < 3; i++) {
        brush->currentColor[i] = currentColor[i];
    }
}

/**
 * @brief Gets the current color of a brush.
 * 
 * @param brush Pointer to the Brush struct whose current color will be retrieved.
 * @return Pointer to an array containing the current color components (RGB values).
 *         It is an array of size 3 containing the red, green, and blue components, respectively.
 */
int* getCurrentColor(Brush * brush) {
    return brush -> currentColor;
}

/**
 * @brief Sets the position of the brush.
 * 
 * @param brush Pointer to the Brush instance.
 * @param pos Pointer to an integer array containing the x and y coordinates of the position.
 */
void setBrushPos(Brush *brush, int* pos) {
    for (int i = 0; i < 2; i++) {
        brush -> brushPos[i] = pos[i];
    }
}

/**
 * @brief Gets the position of the brush.
 * 
 * @param brush Pointer to the Brush instance.
 * @return Pointer to an integer array containing the x and y coordinates of the brush position.
 */
int* getBrushPos(Brush *brush) {
    return brush -> brushPos;
}

/**
 * @brief Constructor function for creating a Brush instance.
 * 
 * @param name Name to set for the brush.
 * @param mode Mode to set for the brush.
 * @param size Size to set for the brush.
 * @param log Pointer to the log for error handling.
 * @return Pointer to the newly created Brush instance.
 */
Brush* brushConstructor(char* name, int mode, int size, Log* log) {
    Brush* brush = malloc(sizeof(Brush));
    if (!brush) {
        logError(log, 310, "Memory Allocation Error");
        exit(EXIT_FAILURE);
    }

    brush -> setBrushName = &setBrushName;
    brush -> getBrushName = &getBrushName;
    brush -> setBrushMode = &setBrushMode;
    brush -> getBrushMode = &getBrushMode;
    brush -> setBrushSize = &setBrushSize;
    brush -> getBrushSize = &getBrushSize;
    brush -> setBrushDrawMode = &setBrushDrawMode;
    brush -> getBrushDrawMode = &getBrushDrawMode;
    brush -> setColorBrush = &setColorBrush;
    brush -> getColorBrush = &getColorBrush;
    brush -> setCurrentColor = &setCurrentColor;
    brush -> getCurrentColor = &getCurrentColor;
    brush -> setBrushPos = &setBrushPos;
    brush -> getBrushPos = &getBrushPos;

    setBrushName(brush, "brush");
    setBrushMode(brush, ID_FREE_MODE);
    setBrushSize(brush, ID_BRUSH_MIN);
    setBrushDrawMode(brush, ID_BRUSH_SQUARE_MODE);
    int defaultColor[3] = {0,0,0};
    setCurrentColor(brush, defaultColor);

    return brush;
}

/**
 * @brief Destructor function for cleaning up resources used by a Brush instance.
 * 
 * @param brush Pointer to the Brush instance to be destroyed.
 */
void brushDeconstructor(Brush* brush) {
    free(brush -> brushName);
    free(brush);
}

/**
 * @brief Structure to hold parameters passed to the window procedure.
 * 
 * This structure contains various parameters that are passed from the WinMain function
 * to the window procedure (WindowProc). These parameters include a logger object, 
 * color table object, brush object, and handles to the status bar and progress bar windows.
 */
typedef struct winParams {
    Log logger;                  /**< Logger object for logging messages and errors. */
    ColorTable *colorTable;      /**< Pointer to the color table object for managing colors. */
    Brush *brush;                /**< Pointer to the brush object for drawing operations. */
    HWND hStatusBar;             /**< Handle to the status bar window. */
    HWND hBrushSlider;
    HINSTANCE hInstance;
} winParams;

typedef struct {
    char text[1000]; // Maximum length of the text
    int numLines;    // Number of lines in the text
} TextProperties;

// Function Prototype.   
void drawCustomLine(HWND hwnd, Brush * brush,  int startX, int startY, int endX, int endY, Log * logger); // Draws a custom line on the specified window using the provided brush, starting from (startX, startY) to (endX, endY).
LRESULT CALLBACK ProgressDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);               // Callback function for the main window procedure.
LRESULT CALLBACK WindowProc(HWND mainHWND, UINT uMsg, WPARAM wParam, LPARAM lParam);                      // Handles messages related to the main window.
const char* getClosestColorName(ColorTable * colorTable, int r, int g, int b);                            // Returns the name of the closest color in the provided color table based on the RGB values.
void updateStatusBarText(Brush * brush, HWND hStatusBar);                                                 // Updates the status bar text based on the current brush settings.
void UpdateProgressBar(HWND hProgressBar, int progress);                                                  // Updates the progress bar with the specified progress value.
void capturePixelData(FILE *file, HWND hwnd, Log * log);                                                  // Captures pixel data from the specified window and writes it to the given file.
void drawPixel(HWND hwnd, Brush * brush, int x, int y);                                                   // Draws a pixel at the specified coordinates on the window using the provided brush.
void loadPixelData(FILE *file, HWND hwnd, Log * log);                                                     // Loads pixel data from the given file and renders it on the specified window.
HWND* ShowProgressDialog(HWND hwndParent, Log * log);                                                     // Displays a progress dialog as a child window of the specified parent window.
BOOL intersect(RECT rect, int x, int y, int offset);                                                      // Checks if the specified point (x, y) intersects with the given rectangle with an optional offset.
void setColor(Brush * brush, int r, int g, int b);                                                        // Sets the color of the provided brush to the specified RGB values.
void CloseProgressDialog(HWND hProgressDialog);                                                           // Closes and destroys the progress dialog window.
char* GetCurrentModeText(Brush * brush);                                                                  // Retrieves the current mode text associated with the provided brush.
void resetColorTextField(HWND hwnd);                                                                      // Resets the color text field to its default state.
void resetCanvas(HWND hwnd);                                                                              // Resets the canvas by clearing all drawn elements from the specified window.



void CreateMiniDump(EXCEPTION_POINTERS* pep) {
    // Open the file for writing
    HANDLE hFile = CreateFileA("MiniDump.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        // Handle error
        return;
    }

    // Write the mini dump
    MINIDUMP_EXCEPTION_INFORMATION mdei;
    mdei.ThreadId = GetCurrentThreadId();
    mdei.ExceptionPointers = pep;
    mdei.ClientPointers = FALSE;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mdei, NULL, NULL);

    // Close the file
    CloseHandle(hFile);
}

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo) {
    CreateMiniDump(ExceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}

/**
 * @brief Main entry point for the application.
 * 
 * @param mainInstance Handle to the current instance of the application.
 * @param prevInstance Reserved parameter, not used.
 * @param lpCmdLine Command-line parameters, not used in this application.
 * @param nCmdShow Specifies how the window is to be shown.
 * @return The exit code returned when the application terminates.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);  // Set custom unhandled exception filter.

    // Initialize logging.
    Log logger;
    initLog(&logger, "logfile.txt");

    // Initialize the color table object.
    ColorTable * colorTable = colorTableConstructor(logger);
    colorTable -> setColorDataFile(colorTable, "./assets/colormap.csv", logger);
    loadColorTableFromCSV(colorTable, logger);

    // Initialize the brush object.
    Brush * brush = brushConstructor("brush", ID_FREE_MODE, ID_BRUSH_MIN, &logger);

    // Register window class.
    WNDCLASS windowClass = { 0 };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hbrBackground = CreateSolidBrush(RGB(255,255,255)); 
    windowClass.lpszClassName = TEXT("PaintWindowClass"); 

    if (!RegisterClass(&windowClass)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the main window.
    HWND mainHWND = CreateWindowEx(
        0,
        TEXT("PaintWindowClass"),
        TEXT("Paint Program | By William (T1WiLLi) | Version: 2024-02-12/4"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL);
    
    if (mainHWND == NULL) {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("ERROR"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Load the application icon.
    HICON hIcon = (HICON) LoadImage(hInstance, "./assets/icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    // Set the icon for the window.
    SendMessage(mainHWND, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(mainHWND, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

    // Create custom slider for brush size.
    HWND hBrushSlider = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 550, 45, BRUSH_SLIDER_WIDTH, BRUSH_SLIDER_HEIGHT, mainHWND, (HMENU)ID_BRUSH_SLIDER, hInstance, NULL);
    SendMessage(hBrushSlider, TBM_SETRANGE, TRUE, MAKELPARAM(1, 26));
    SendMessage(hBrushSlider, TBM_SETPOS, TRUE, brush -> getBrushSize(brush));

    // Create bottom status bar.
    HWND hStatusBar = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        NULL,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0,0,0,0,
        mainHWND,
        NULL,
        hInstance,
        NULL);
    
    int parts[] = {80, 230, 360, 530, 750, 870, -1};
    SendMessage(hStatusBar, SB_SETPARTS, sizeof(parts) / sizeof(parts[0]), (LPARAM)parts);

    // Set up window parameters.
    winParams params;
    params.logger = logger;
    params.brush = brush;
    params.colorTable = colorTable;
    params.hStatusBar = hStatusBar;
    params.hBrushSlider = hBrushSlider;
    params.hInstance = hInstance;

    // Set the Params structure as the user data associated with the mainHWND.
    SetWindowLongPtr(mainHWND, GWLP_USERDATA, (LONG_PTR)&params);
    SendMessage(mainHWND, WM_CREATE, 0, 0);

    // Show the How To Windows
    ShowHowToDialog(mainHWND);

    // Show and update the main window.
    ShowWindow(mainHWND, nCmdShow);
    UpdateWindow(mainHWND);

    // Message loop.
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up resources.
    closeLog(&logger);
    brushDeconstructor(brush);
    colorTableDeconstructor(colorTable);
    DestroyIcon(hIcon);
    return msg.wParam;
}

/**
 * @brief Window procedure for handling window messages.
 * 
 * @param mainHWND Handle to the main application window.
 * @param uMsg The message to be processed.
 * @param wParam Additional message-specific information.
 * @param lParam Additional message-specific information.
 * @return The result of the message processing and depends on the message type.
 */
LRESULT CALLBACK WindowProc(HWND mainHWND, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Retrieve the Params structure from the user data associated with the window handle
    winParams* params = (winParams*)GetWindowLongPtr(mainHWND, GWLP_USERDATA);

    // Ensure that the Params structure is valid
    if (params == NULL) {
        return DefWindowProc(mainHWND, uMsg, wParam, lParam);
    }

    // Retrieve objs and data
    Log logger = params -> logger;
    ColorTable * colorTable = params -> colorTable;
    Brush * brush = params -> brush;
    HWND hStatusBar = params -> hStatusBar;
    HWND hBrushSlider = params -> hBrushSlider;
    HINSTANCE hInstance = params -> hInstance;

    // Init all runtime values
    static TextProperties textProperties = { "", 1};
    static POINT textStartPoint = {0};
    static char textBuffer[256] = {0};
    static POINT startPoint; // Stay the same and has the same adr throughout the program
    static BOOL useCustomColor = FALSE; // Idem
    static int customColor[3]; // Default black value.
    static int lastUsedColor[3];// last used color
    FILE* savingFile;

    COLORREF buttonColors[] = {
        RGB(0, 0, 0),   // Black
        RGB(255, 0, 0), // Red
        RGB(0, 255, 0), // Green
        RGB(0, 0, 255),  // Blue
        RGB(255,255,0), // Yellow
        RGB(255, 165, 0), // Orange
        RGB(128, 0, 128), // Purple
        RGB(128, 128, 128), // Gray
        RGB(165, 42, 42) // Brown
    };

    // Load the default cross cursor
    HCURSOR hCustomCursor = LoadCursor(NULL, IDC_CROSS);
    if (hCustomCursor == NULL) {
        DWORD error = GetLastError();
        logError(&logger, 571, "Failed to load cursor. Error code: %lu", error);
    }

    // Set the cursor
    SetCursor(hCustomCursor);

    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT painter;
            HDC hdc = BeginPaint(mainHWND, &painter);
            RECT currRect;
            GetClientRect(mainHWND, &currRect);

            RECT divRect = {
                currRect.left,
                currRect.top,
                currRect.right,
                currRect.top + TOOLBAR_HEIGHT
            };

            HBRUSH hBrush = CreateSolidBrush(RGB(0,0,150));
            FillRect(hdc, &divRect, hBrush);
            DeleteObject(hBrush); // Free memory

            // Create a font with the desired size
            SetBkMode(hdc, TRANSPARENT);
            HFONT hFontSmall = CreateFont(
                30,                   // Font height
                0,                    // Font width (0 to let Windows choose the best width)
                0,                    // Escapement angle
                0,                    // Orientation angle
                FW_DEMIBOLD,          // Font weight
                FALSE,                // Italic
                FALSE,                 // Underline
                FALSE,                // Strikeout
                ANSI_CHARSET,         // Character set
                OUT_DEFAULT_PRECIS,   // Output precision
                CLIP_DEFAULT_PRECIS,  // Clipping precision
                DEFAULT_QUALITY,      // Output quality
                DEFAULT_PITCH | FF_DONTCARE, // Pitch and family
                TEXT("Arial")         // Font name
            );

            HFONT hFontLarge = CreateFont(
                60,                   // Font height
                0,                    // Font width (0 to let Windows choose the best width)
                0,                    // Escapement angle
                0,                    // Orientation angle
                FW_DEMIBOLD,          // Font weight
                FALSE,                // Italic
                FALSE,                 // Underline
                FALSE,                // Strikeout
                ANSI_CHARSET,         // Character set
                OUT_DEFAULT_PRECIS,   // Output precision
                CLIP_DEFAULT_PRECIS,  // Clipping precision
                DEFAULT_QUALITY,      // Output quality
                DEFAULT_PITCH | FF_DONTCARE, // Pitch and family
                TEXT("Arial")         // Font name
            );

            // Use smaller font for the first TextOut
            HFONT hOldFont = SelectObject(hdc, hFontLarge);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOut(hdc, 10, 10, TEXT("Paint-C"), lstrlen(TEXT("Paint-C")));
            SelectObject(hdc, hOldFont);

            // Use larger font for the second TextOut
            hOldFont = SelectObject(hdc, hFontSmall);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOut(hdc, 585, 10, TEXT("Brush Size"), lstrlen(TEXT("Brush Size")));
            SelectObject(hdc, hOldFont);

            // Delete fonts when they are no longer needed
            DeleteObject(hFontSmall);
            DeleteObject(hFontLarge);

            HICON hIcon = (HICON)LoadImage(NULL, "./assets/icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
            if (hIcon != NULL) {
                int xPos = 200;  // X position in pixels
                int yPos = (TOOLBAR_HEIGHT / 2 ) - (64 / 2);  // Y position in pixels

                int iconWidth = 64;  // Width of the icon in pixels
                int iconHeight = 64; // Height of the icon in pixels

                // Draw the icon at the specified position and size
                DrawIconEx(hdc, xPos, yPos, hIcon, iconWidth, iconHeight, 0, NULL, DI_NORMAL);
                DestroyIcon(hIcon);
            } else {
                logDebug(&logger, "Hicon is NULL");
            }

            SetBkMode(hdc, OPAQUE);
            for(int i = 0; i < COLOR_BUTTON_GRID_SIZE * COLOR_BUTTON_GRID_SIZE; ++i) {
                int buttonX = COLOR_GRID_OFFSET_X + (i % COLOR_BUTTON_GRID_SIZE) * (COLOR_BUTTON_WIDTH + COLOR_BUTTON_SPACING) + COLOR_BUTTON_SPACING;
                int buttonY = (i / COLOR_BUTTON_GRID_SIZE) * (COLOR_BUTTON_HEIGHT + COLOR_BUTTON_SPACING) + COLOR_BUTTON_SPACING;
                RECT colorBorder = {
                    buttonX - 1, buttonY - 1,
                    buttonX + COLOR_BUTTON_WIDTH + 1, buttonY + COLOR_BUTTON_HEIGHT + 1
                };
                HBRUSH hColorBorderBrush = CreateSolidBrush(RGB(255,255,255));
                FillRect(hdc, &colorBorder, hColorBorderBrush);
                DeleteObject(hColorBorderBrush);
            }

            if (brush->getBrushMode(brush) == ID_TEXT_MODE && GetFocus() == mainHWND) {

                // Calculate the position for the blinking bar
                int barX = textStartPoint.x; // Adjust as needed
                int barY = textStartPoint.y; // Adjust as needed
                int barWidth = 2; // Adjust as needed
                int barHeight = brush -> getBrushSize(brush); // Adjust as needed

                // Draw the blinking bar as a thin rectangle
                RECT barRect = { barX, barY, barX + barWidth, barY + barHeight };
                FillRect(hdc, &barRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

                RECT clientRect;
                GetClientRect(mainHWND, &clientRect);
                HFONT hFont = CreateFont(
                    brush -> getBrushSize(brush), // Font height
                    0,                  // Font width
                    0,                  // Escapement angle
                    0,                  // Orientation angle
                    FW_NORMAL,          // Font weight
                    FALSE,              // Italic
                    FALSE,              // Underline
                    FALSE,              // Strikeout
                    ANSI_CHARSET,       // Character set
                    OUT_DEFAULT_PRECIS, // Output precision
                    CLIP_DEFAULT_PRECIS,// Clipping precision
                    DEFAULT_QUALITY,    // Output quality
                    DEFAULT_PITCH,      // Pitch and family
                    TEXT("Arial")       // Font name
                );
                HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

                // Set text color
                SetTextColor(hdc, RGB(0, 0, 0)); // Black color

                // Set text alignment
                SetTextAlign(hdc, TA_LEFT | TA_TOP);

                // Calculate the rectangle for text rendering
                RECT textRect;
                textRect.left = textStartPoint.x;
                textRect.top = textStartPoint.y;
                textRect.right = clientRect.right; // Adjust the right coordinate as needed
                textRect.bottom = clientRect.bottom; // Adjust the bottom coordinate as needed

                // Draw the text
                DrawText(hdc, textBuffer, -1, &textRect, DT_LEFT | DT_WORDBREAK);

                // Clean up
                SelectObject(hdc, hOldFont);
                DeleteObject(hFont);
            }
            EndPaint(mainHWND, &painter);
            break;
        }
        case WM_CREATE: {
            for(int i = 0; i < 9; ++i) {
                int buttonX = COLOR_GRID_OFFSET_X + (i % 3) * (COLOR_BUTTON_WIDTH + COLOR_BUTTON_SPACING) + COLOR_BUTTON_SPACING;  // Adjusted for 3 buttons per row
                int buttonY = (i / 3) * (COLOR_BUTTON_HEIGHT + COLOR_BUTTON_SPACING) + COLOR_BUTTON_SPACING; // Adjusted for 3 buttons per row

                HWND hButton = CreateWindow(TEXT("BUTTON"), TEXT("Color"), WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                            buttonX, buttonY, COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT,
                                            mainHWND, (HMENU)(intptr_t)(ID_COLOR_BLACK + i),
                                            hInstance, NULL);

                // Draw a border around the button
                RECT colorBorder = {
                    buttonX - 1, buttonY - 1,
                    buttonX + COLOR_BUTTON_WIDTH + 1, buttonY + COLOR_BUTTON_HEIGHT + 1
                };
                HBRUSH hColorBorderBrush = CreateSolidBrush(RGB(255,255,255));
                FillRect(GetDC(mainHWND), &colorBorder, hColorBorderBrush);
                DeleteObject(hColorBorderBrush);
            }

            // Custom color textfield and button!
            HWND hEdit = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | SS_CENTER, 360, 15, 180, 20, mainHWND, (HMENU)ID_CUSTOM_COLOR_LABEL, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("CUSTOM COLOR"), WS_VISIBLE | WS_CHILD, 360, 45, 180, 30, mainHWND, (HMENU)ID_CUSTOM_BUTTON_COLOR, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Eraser"), WS_VISIBLE | WS_CHILD, 760, 10, 80, 30, mainHWND, (HMENU)ID_ERASER_MODE, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Reset"), WS_VISIBLE | WS_CHILD | SS_CENTER, 760, 45, 80, 30, mainHWND, (HMENU) ID_RESET, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Pixel Mode"), WS_VISIBLE | WS_CHILD, 850, 10, 100, 30, mainHWND, (HMENU)ID_GRID_MODE, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Line Mode"), WS_VISIBLE | WS_CHILD | SS_CENTER, 850, 45, 100, 30, mainHWND, (HMENU) ID_LINE_MODE, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Text Mode"), WS_VISIBLE | WS_CHILD | SS_CENTER, 960, 10, 100, 30, mainHWND, (HMENU) ID_TEXT_MODE, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Free Mode"), WS_VISIBLE | WS_CHILD | SS_CENTER, 960, 45, 100, 30, mainHWND, (HMENU) ID_FREE_MODE, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("S-Draw Mode"), WS_VISIBLE | WS_CHILD | SS_CENTER, 1070, 10, 100, 30, mainHWND, (HMENU) ID_BRUSH_SQUARE_MODE, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("C-Draw Mode"), WS_VISIBLE | WS_CHILD | SS_CENTER, 1070, 45, 100, 30, mainHWND, (HMENU) ID_BRUSH_CIRCLE_MODE, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("SAVE"), WS_VISIBLE | WS_CHILD | SS_CENTER, 1180, 10, 80, 30, mainHWND, (HMENU) ID_SAVE_BUTTON, hInstance, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("LOAD"), WS_VISIBLE | WS_CHILD | SS_CENTER, 1180, 45, 80, 30, mainHWND, (HMENU) ID_LOAD_BUTTON, hInstance, NULL);
            break;
        }
        case WM_CTLCOLORBTN: {
            HDC hdcButton = (HDC)wParam;
            HWND hButton = (HWND)lParam;
            int buttonID = GetDlgCtrlID(hButton);
            SetBkColor(hdcButton, buttonColors[buttonID - ID_COLOR_BLACK]);
            return (LRESULT)CreateSolidBrush(buttonColors[buttonID - ID_COLOR_BLACK]);
        }
        case WM_LBUTTONDOWN: {
            startPoint.x = LOWORD(lParam);
            startPoint.y = HIWORD(lParam);
            if (brush->getBrushMode(brush) != ID_LINE_MODE && brush->getBrushMode(brush) != ID_TEXT_MODE) {
                drawPixel(mainHWND, brush, startPoint.x, startPoint.y);
            } else if (brush->getBrushMode(brush) == ID_TEXT_MODE) {
                textStartPoint.x = LOWORD(lParam);
                textStartPoint.y = HIWORD(lParam);

                // Clear the existing text buffer
                textBuffer[0] = '\0';
                textProperties.numLines = 1;

                SetFocus(mainHWND);
                InvalidateRect(mainHWND, NULL, TRUE);
            }
            break;
        }
        case WM_LBUTTONUP: {
            if (brush -> getBrushMode(brush) == ID_LINE_MODE) {
                POINT endPoint;
                endPoint.x = LOWORD(lParam);
                endPoint.y = HIWORD(lParam);
                drawCustomLine(mainHWND, brush, startPoint.x, startPoint.y, endPoint.x, endPoint.y, &logger);
            }
            break;
        }
        case WM_MOUSEMOVE: {
            int pos[] = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            brush -> setBrushPos(brush, pos);
    
            if (wParam & MK_LBUTTON && (brush -> getBrushMode(brush) != ID_LINE_MODE)) {
                POINT currentPoint;
                currentPoint.x = LOWORD(lParam);
                currentPoint.y = HIWORD(lParam);
                drawPixel(mainHWND, brush, currentPoint.x, currentPoint.y);
                startPoint = currentPoint;
            }
            break;
        }
        case WM_HSCROLL: {
            if ((HWND)lParam == hBrushSlider) {
                int newPos = SendMessage(hBrushSlider, TBM_GETPOS, 0, 0);
                int roundedPos = (newPos / 2) * 2; // Round to nearest multiple of 2

                if (roundedPos != brush -> getBrushSize(brush)) {
                    brush -> setBrushSize(brush, roundedPos);
                    SendMessage(hBrushSlider, TBM_SETPOS, TRUE, brush -> getBrushSize(brush));
                }
            }
            break;
        }
        // Inside the WM_CHAR message handler
        case WM_CHAR: {
            if (brush->getBrushMode(brush) == ID_TEXT_MODE) {
                char inputChar = (char)wParam;
                if (inputChar == '\r') {
                    // Handle newline
                    if (textProperties.numLines < 20) { // Maximum 20 lines for demonstration
                        strcat(textBuffer, "\r\n"); // Append newline characters to the existing buffer
                        textProperties.numLines++; // Increment line count
                        InvalidateRect(mainHWND, NULL, TRUE); // Redraw window
                    }
                } else if (inputChar == '\b') {
                    // Handle backspace
                    int len = strlen(textBuffer);
                    if (len > 0) {
                        textBuffer[len - 1] = '\0'; // Remove last character
                        InvalidateRect(mainHWND, NULL, TRUE); // Redraw window
                    }
                } else {
                    // Append the character to the text buffer
                    int len = strlen(textBuffer);
                    if (len < sizeof(textBuffer) - 1) {
                        textBuffer[len] = inputChar;
                        textBuffer[len + 1] = '\0';
                        InvalidateRect(mainHWND, NULL, TRUE); // Redraw window
                    }
                }
            }
            break;
        }
        case WM_COMMAND: {

            if (LOWORD(wParam) == ID_CUSTOM_COLOR_LABEL && HIWORD(wParam) == EN_CHANGE) {
                char buffer[12];
                GetWindowText(GetDlgItem(mainHWND, ID_CUSTOM_COLOR_LABEL), buffer, sizeof(buffer));
                int r,g,b;
                if (sscanf(buffer, "%d,%d,%d", &r, &g, &b) == 3) {
                    customColor[0] = r;
                    customColor[1] = g;
                    customColor[2] = b;
                    
                    const char* closestColorName = getClosestColorName(colorTable, r, g, b);
                    SetWindowText(GetDlgItem(mainHWND, ID_CUSTOM_BUTTON_COLOR), closestColorName);
                    if (useCustomColor) {
                        setColor(brush, r, g, b);
                    }
                    logDebug(&logger, "New RGB Value: %d, %d, %d", r,g,b);
                }
                logDebug(&logger, "Text Value changed: %s", buffer);
            }
            if (brush -> getBrushMode(brush) != ID_ERASER_MODE) {
                int tempColor[] = {brush -> getCurrentColor(brush)[0], brush -> getCurrentColor(brush)[1], brush -> getCurrentColor(brush)[2]};
                for(int i = 0; i < 3; i++) lastUsedColor[i] = tempColor[i];
            }
            switch(LOWORD(wParam)) {
                case ID_COLOR_BLACK: {
                    setColor(brush, 0, 0, 0);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_COLOR_RED: {
                    setColor(brush, 255, 0, 0);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_COLOR_GREEN: {
                    setColor(brush, 0, 255, 0);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_COLOR_BLUE: {
                    setColor(brush, 0, 0, 255);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_COLOR_YELLOW: {
                    setColor(brush, 255, 255, 0);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_COLOR_ORANGE: {
                    setColor(brush, 255, 165, 0);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_COLOR_PURPLE: {
                    setColor(brush, 128, 0, 128);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_COLOR_GRAY: {
                    setColor(brush, 128, 128, 128);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_COLOR_BROWN: {
                    setColor(brush, 165, 42, 42);
                    resetColorTextField(mainHWND);
                    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
                        brush -> setBrushMode(brush, ID_FREE_MODE);
                    }
                    break;
                }
                case ID_CUSTOM_BUTTON_COLOR: {
                    useCustomColor = !useCustomColor;
                    if (useCustomColor) {
                        setColor(brush, customColor[0], customColor[1], customColor[2]);
                    }
                    logDebug(&logger, "State of the custom button: %d", (useCustomColor == TRUE) ? 1 : 0);
                    break;
                }
                case ID_FREE_MODE: {
                    brush -> setBrushMode(brush, ID_FREE_MODE);
                    setColor(brush, lastUsedColor[0], lastUsedColor[1], lastUsedColor[2]);
                    break;
                }
                case ID_TEXT_MODE: {
                    brush -> setBrushMode(brush, ID_TEXT_MODE);
                    setColor(brush, lastUsedColor[0], lastUsedColor[1], lastUsedColor[2]);
                    break;
                }
                case ID_ERASER_MODE: {
                    brush -> setBrushMode(brush, ID_ERASER_MODE);
                    int tempColor[] = {brush -> getCurrentColor(brush)[0], brush -> getCurrentColor(brush)[1], brush -> getCurrentColor(brush)[2]};
                    for(int i = 0; i < 3; i++) lastUsedColor[i] = tempColor[i];
                    setColor(brush, 255,255,255);
                    break;
                }
                case ID_GRID_MODE: {
                    brush -> setBrushMode(brush, ID_GRID_MODE);
                    setColor(brush, lastUsedColor[0], lastUsedColor[1], lastUsedColor[2]);
                    break;
                }
                case ID_LINE_MODE: {
                    brush -> setBrushMode(brush, ID_LINE_MODE);
                    setColor(brush, lastUsedColor[0], lastUsedColor[1], lastUsedColor[2]);
                    break;
                }
                case ID_BRUSH_SQUARE_MODE: {
                    brush -> setBrushDrawMode(brush, ID_BRUSH_SQUARE_MODE);
                    break;
                }
                case ID_BRUSH_CIRCLE_MODE: {
                    brush -> setBrushDrawMode(brush, ID_BRUSH_CIRCLE_MODE);
                    break;
                }
                case ID_RESET: {
                    resetCanvas(mainHWND);
                    break;
                }
                case ID_SAVE_BUTTON: {
                    DWORD start = GetTickCount();
                    logDebug(&logger, "Saving started...");
                    savingFile = fopen("./assets/pixel_data.csv", "w");
                    if (savingFile == NULL) {
                        logError(&logger, 584, "Failed to open file for writing");
                        break;
                    }
                    capturePixelData(savingFile, mainHWND, &logger);
                    fclose(savingFile);

                    DWORD end = GetTickCount();
                    DWORD duration = end - start;
                    logDebug(&logger, "Saving done.");
                    char durationStr[256];
                    logDebug(&logger, "Time taken for saving: %lu milliseconds", duration);
                    break;
                }
                case ID_LOAD_BUTTON: {
                    DWORD start = GetTickCount();
                    logDebug(&logger, "Loading started...");
                    savingFile = fopen("./assets/pixel_data.csv", "r");
                    if (savingFile == NULL) {
                        logError(&logger, 602, "Failed to open file for reading");
                        break;
                    }
                    loadPixelData(savingFile, mainHWND, &logger);
                    fclose(savingFile);

                    DWORD end = GetTickCount();
                    DWORD duration = end - start;
                    logDebug(&logger, "Loading done.");
                    char durationStr[256];
                    logDebug(&logger, "Time taken for loading: %lu milliseconds", duration);
                    break;
                }
            }
            break;
        }
        case WM_DESTROY: {
            DestroyCursor(hCustomCursor);
            PostQuitMessage(0);
        }
        default: {
            return DefWindowProc(mainHWND, uMsg, wParam, lParam);
        }
    }
    char buffer[50];
    wsprintf(buffer, TEXT("Brush Size: %d"), brush -> getBrushSize(brush));
    updateStatusBarText(brush, hStatusBar);
    return 0;
}

/**
 * @brief Resets the canvas background color to white.
 * 
 * @param hwnd Handle to the window whose canvas background color will be reset.
 */

void resetCanvas(HWND hwnd) {
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR) CreateSolidBrush(RGB(255,255,255)));
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

/**
 * @brief Sets the color of the brush and updates the current color.
 * 
 * @param brush Pointer to the Brush instance.
 * @param r Red component of the new color (0-255).
 * @param g Green component of the new color (0-255).
 * @param b Blue component of the new color (0-255).
 */
void setColor(Brush * brush, int r, int g, int b) {
    int colors[] = {r,g,b};
    DeleteObject(brush -> getColorBrush(brush));
    brush -> setColorBrush(brush, CreateSolidBrush(RGB(r, g, b)));
    brush -> setCurrentColor(brush, colors);
}

/**
 * @brief Resets the text fields for custom color input.
 * 
 * @param hwnd Handle to the main application window.
 */
void resetColorTextField(HWND hwnd) {
    SetWindowText(GetDlgItem(hwnd, ID_CUSTOM_COLOR_LABEL), "");
    SetWindowText(GetDlgItem(hwnd, ID_CUSTOM_BUTTON_COLOR), "CUSTOM COLOR");
}

/**
 * @brief Checks if a point (x, y) lies within a rectangle with an optional offset.
 * 
 * @param rect The rectangle to check against.
 * @param x The x-coordinate of the point.
 * @param y The y-coordinate of the point.
 * @param offset The offset to be added to the bottom boundary of the rectangle.
 * @return TRUE if the point lies within the rectangle (considering the offset), otherwise FALSE.
 */
BOOL intersect(RECT rect, int x, int y, int offset) {
    if (x >= rect.left && x < rect.right && y >= rect.top && y < (rect.bottom + offset)) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Draws a pixel on the canvas.
 * 
 * @param hwnd Handle to the window whose canvas the pixel will be drawn on.
 * @param brush Pointer to the Brush struct containing the brush settings.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 */
void drawPixel(HWND hwnd, Brush * brush, int x, int y) {
    int halfsize = brush -> getBrushSize(brush) / 2;
    HDC hdc = GetDC(hwnd);

    RECT currRect;
    GetClientRect(hwnd, &currRect);
    RECT clientRect = { currRect.left, currRect.top, currRect.right, currRect.top + TOOLBAR_HEIGHT};
    if (intersect(clientRect, x, y, halfsize)) {
        ReleaseDC(hwnd, hdc);
        return;
    }

    if (brush -> getBrushMode(brush) == ID_GRID_MODE) {
        int gridSize = brush -> getBrushSize(brush);
        x = ((x + gridSize / 2) / gridSize) * gridSize;
        y = ((y + gridSize / 2) / gridSize) * gridSize;
    }

    LOGBRUSH lb;
    GetObject(brush -> getColorBrush(brush), sizeof(LOGBRUSH), &lb);

    if (!(brush -> getBrushMode(brush)) == ID_ERASER_MODE) {
        brush -> getCurrentColor(brush)[0] = GetRValue(lb.lbColor);
        brush -> getCurrentColor(brush)[1] = GetGValue(lb.lbColor);
        brush -> getCurrentColor(brush)[2] = GetBValue(lb.lbColor);
    }

    if (brush -> getBrushDrawMode(brush) == ID_BRUSH_SQUARE_MODE) {
        // Squared Pixel
        for(int i = -halfsize; i <= halfsize; ++i) {
            for(int j = -halfsize; j <= halfsize; ++j) {
                SetPixel(hdc, x + i, y + j, RGB(brush -> getCurrentColor(brush)[0], brush -> getCurrentColor(brush)[1], brush -> getCurrentColor(brush)[2]));
            }
        }
    } else {
        // Draw circle
        for (int i = -halfsize; i <= halfsize; ++i) {
            for (int j = -halfsize; j <= halfsize; ++j) {
                if (i * i + j * j <= halfsize * halfsize) {
                    SetPixel(hdc, x + i, y + j, RGB(brush->getCurrentColor(brush)[0], brush->getCurrentColor(brush)[1], brush->getCurrentColor(brush)[2]));
                }
            }
        }
    }

    ReleaseDC(hwnd, hdc);
}

void drawCustomLine(HWND hwnd, Brush * brush, int startX, int startY, int endX, int endY, Log * logger) {
    HDC hdc = GetDC(hwnd);
    int halfSize = brush -> getBrushSize(brush) / 2;

    logDebug(logger, "Sx: %d, Sy: %d, Ex: %d, Ey: %d", startX, startY, endX, endY);

    RECT currRect;
    GetClientRect(hwnd, &currRect);
    RECT divRect = { currRect.left, currRect.top, currRect.right, currRect.top + TOOLBAR_HEIGHT };

    if (intersect(divRect, startX, startY, halfSize)) {
        ReleaseDC(hwnd, hdc);
        return;
    } else if (intersect(divRect, endX, endY, halfSize)) {
        endY = (currRect.top + TOOLBAR_HEIGHT - 1) + halfSize;
    }

    for (int i = -halfSize; i <= halfSize; ++i) {
        for (int j = -halfSize; j <= halfSize; ++j) {
            int rectStartX = startX + i;
            int rectStartY = startY + j;
            int rectEndX = endX + i;
            int rectEndY = endY + j;

            LOGBRUSH lb;
            GetObject(brush -> getColorBrush(brush), sizeof(LOGBRUSH), &lb);
            
            if (!(brush -> getBrushMode(brush)) == ID_ERASER_MODE) {
                brush -> getCurrentColor(brush)[0] = GetRValue(lb.lbColor);
                brush -> getCurrentColor(brush)[1] = GetGValue(lb.lbColor);
                brush -> getCurrentColor(brush)[2] = GetBValue(lb.lbColor);
            }

            // Draw small rectangles between the starting and ending points with currentColor
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(brush -> getCurrentColor(brush)[0], brush -> getCurrentColor(brush)[1], brush -> getCurrentColor(brush)[2]));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

            MoveToEx(hdc, rectStartX, rectStartY, NULL);
            LineTo(hdc, rectEndX, rectEndY);

            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }
    }
    ReleaseDC(hwnd, hdc);
}

/**
 * @brief Updates the status bar text with information about the brush.
 * 
 * @param brush Pointer to the Brush instance.
 * @param hStatusBar Handle to the status bar window.
 */
void updateStatusBarText(Brush * brush, HWND hStatusBar) {
    char brushSizeText[256];
    char colorRGBValueText[256];
    char currentModeText[256];
    char currentMousePositionText[256];
    char copyright[256];
    sprintf_s(brushSizeText, sizeof(brushSizeText), "Brush Size: %d", brush -> getBrushSize(brush));
    sprintf_s(colorRGBValueText, sizeof(colorRGBValueText), "Color: RGB(%d, %d, %d)", brush -> getCurrentColor(brush)[0], brush -> getCurrentColor(brush)[1], brush -> getCurrentColor(brush)[2]);
    sprintf_s(currentModeText, sizeof(currentModeText), "Current Mode: %s", GetCurrentModeText(brush));
    sprintf_s(currentMousePositionText, sizeof(currentMousePositionText), "Mouse Pos = X: %d, Y: %d", brush -> getBrushPos(brush)[0], brush -> getBrushPos(brush)[1]);
    sprintf_s(copyright, sizeof(copyright), "Copyright William Beaudin 2024");

    SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)brushSizeText);
    SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)colorRGBValueText);
    SendMessage(hStatusBar, SB_SETTEXT, 2, (LPARAM)currentModeText);
    SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM)currentMousePositionText);
    SendMessage(hStatusBar, SB_SETTEXT, 4, (LPARAM)copyright);
}

/**
 * @brief Returns the text representation of the current mode of the brush.
 * 
 * @param brush Pointer to the Brush instance.
 * @return Text representation of the current brush mode.
 */
char* GetCurrentModeText(Brush * brush) {
    if (brush -> getBrushMode(brush) == ID_ERASER_MODE) {
        return "ERASE";
    } else if (brush -> getBrushMode(brush) == ID_GRID_MODE) {
        return "PIXEL";
    } else if (brush -> getBrushMode(brush) == ID_LINE_MODE) {
        return "LINE";
    } else if (brush -> getBrushMode(brush) == ID_TEXT_MODE) {
        return "TEXT";
    } else {
        return "FREE";
    }
}

/**
 * @brief Finds the name of the color closest to the specified RGB values in the color table.
 * 
 * @param colorTable Pointer to the ColorTable instance containing color mappings.
 * @param r The red component of the target color (0-255).
 * @param g The green component of the target color (0-255).
 * @param b The blue component of the target color (0-255).
 * 
 * @return A pointer to the name of the color closest to the specified RGB values.
 * If the color table is empty, returns the first color in the list.
 */
const char* getClosestColorName(ColorTable * colorTable, int r, int g, int b) {
    Color* closestMatch = NULL;
    double minDistance = 1.79769e+308;

    for(int i = 0; i < colorTable -> colorCount; ++i) {
        Color* mapping = &colorTable -> colorMappings[i];
        double distance = sqrt(pow(mapping -> r - r, 2) + pow(mapping -> g - g, 2) + pow(mapping -> b - b, 2));

        if (distance < minDistance) {
            minDistance = distance;
            closestMatch = mapping;
        }
    }
    return (closestMatch != NULL) ? closestMatch -> name : colorTable -> colorMappings[0].name;
}

/**
 * @brief Captures pixel data from the window and writes it to a file.
 * 
 * @param file The file pointer to write the pixel data to.
 * @param hwnd The handle to the window whose pixel data needs to be captured.
 */
void capturePixelData(FILE *file, HWND hwnd, Log * log) {
    // Display progress dialog
    HWND* inValue = ShowProgressDialog(hwnd, log);
    if (inValue == NULL) {
        // Handle error
        return;
    }
    HWND hProgressDialog = inValue[0];
    HWND hProgressBar = inValue[1];
    
    // Get device context of the window
    HDC hdc = GetDC(hwnd);
    if (hdc == NULL) {
        return;
    }

    // Get the dimensions of the window's client area
    RECT currRect;
    GetClientRect(hwnd, &currRect);

    // Calculate total number of pixels
    int totalPixels = (currRect.right - currRect.left) * (currRect.bottom - currRect.top);
    int pixelProcessed = 0;
    const int updateInterval = totalPixels / 100;

    // Create a memory device context compatible with the window's device context
    HDC memDC = CreateCompatibleDC(hdc);
    if (memDC == NULL) {
        ReleaseDC(hwnd, hdc);
        return;
    }

    // Create a compatible bitmap to store pixel data
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, currRect.right - currRect.left, currRect.bottom - currRect.top);
    if (hBitmap == NULL) {
        DeleteDC(memDC);
        ReleaseDC(hwnd, hdc);
        return;
    }
    HGDIOBJ hOldBitmap = SelectObject(memDC, hBitmap);

    // Copy the window pixels to the memory device context
    if (!BitBlt(memDC, 0, 0, currRect.right - currRect.left, currRect.bottom - currRect.top, hdc, 0, 0, SRCCOPY)) {
        SelectObject(memDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        ReleaseDC(hwnd, hdc);
        return;
    }

    // Process pixel data
    for (int y = 0; y < currRect.bottom - currRect.top; y++) {
        for (int x = 0; x < currRect.right - currRect.left; x++) {
            // Get the color of the pixel at (x, y)
            COLORREF color = GetPixel(memDC, x, y);
            if (color == CLR_INVALID) {
                continue; // Skip invalid pixels
            }
            
            // Extract RGB values from the color
            BYTE blue = GetBValue(color);
            BYTE green = GetGValue(color);
            BYTE red = GetRValue(color);

            // Write pixel information to the file
            fprintf(file, "%d,%d,%d,%d,%d\n", x, y, red, green, blue);
            pixelProcessed++;
            if (pixelProcessed % updateInterval == 0) {
                int progress = (pixelProcessed * 100) / totalPixels;
                UpdateProgressBar(hProgressBar, progress);
            }
        }
    }

    // Close progress dialog
    CloseProgressDialog(hProgressDialog);
    free(inValue);

    // Clean up resources
    SelectObject(memDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(hwnd, hdc);
}

/**
 * @brief Loads pixel data from a file and sets the corresponding pixels on the window.
 * 
 * @param file The file pointer to read the pixel data from.
 * @param hwnd The handle to the window where pixel data needs to be loaded.
 * @param log Pointer to the log instance for logging errors or debug messages.
 */
void loadPixelData(FILE *file, HWND hwnd, Log *log) {
    // Reset file pointer to the beginning
    rewind(file);

    // Get device context of the window
    HDC hdc = GetDC(hwnd);
    if (hdc == NULL) {
        return;
    }

    // Allocate memory for reading lines from the file
    size_t buffer_size = 64;
    char *buffer = malloc(buffer_size * sizeof(char));

    // Read pixel data from the file and set corresponding pixels on the window
    while(fgets(buffer, buffer_size, file) != NULL) {
        int x, y, r, g, b;
        if (sscanf(buffer, "%d,%d,%d,%d,%d", &x, &y, &r, &g, &b) == 5) {
            if (r != 255 || g != 255 || b != 255) {
                SetPixel(hdc, x, y, RGB(r,g,b));
            }
        }
    }

    // Release allocated resources
    free(buffer);
    ReleaseDC(hwnd, hdc);
}

/**
 * @brief Displays a progress dialog with a progress bar.
 * 
 * @param hwndParent Handle to the parent window.
 * @param hProgressBar Handle to the progress bar window.
 * @param log Pointer to the log for error handling.
 * @return Handle to the created progress dialog window, or NULL if creation fails.
 */
HWND* ShowProgressDialog(HWND hwndParent, Log* log) {
    // Allocate memory for the array
    HWND* returnValues = malloc(2 * sizeof(HWND));
    if (returnValues == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    HWND hProgressBar;
    HWND hProgressDialog = CreateWindowEx(
        WS_EX_DLGMODALFRAME,
        MAKEINTATOM(WC_DIALOG),
        "Saving you're drawing...",
        WS_CAPTION | WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        300, 60,
        hwndParent,
        NULL,
        NULL,
        NULL);

    // Check if the window was created successfully
    if (hProgressDialog != NULL) {
        // Desired position (x, y)
        int x = SCREEN_WIDTH - (SCREEN_WIDTH / 2);
        int y = SCREEN_HEIGHT - (SCREEN_HEIGHT / 2);
        // Set the position
        SetWindowPos(hProgressDialog, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        // Create the progress bar
        hProgressBar = CreateWindowEx(
            0,
            PROGRESS_CLASS,
            NULL,
            WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
            5, 5, 280, 20,
            hProgressDialog,
            (HMENU)ID_PROGRESS_BAR,
            NULL,
            NULL);

        // Show the dialog
        ShowWindow(hProgressDialog, SW_SHOW);
    } else {
        logError(log, 1010, "THE SAVE-WINDOWS COULDN'T BE CREATED AND IT RESULT IN AN AUTOMATIC FAILURE.");
    }

    // Assign values to the array
    returnValues[0] = hProgressDialog;
    returnValues[1] = hProgressBar;

    return returnValues;
}



/**
 * @brief Closes the progress dialog window.
 * 
 * @param hProgressDialog Handle to the progress dialog window to be closed.
 */
void CloseProgressDialog(HWND hProgressDialog) {
    if (hProgressDialog != NULL) {
        DestroyWindow(hProgressDialog);
        hProgressDialog = NULL;
    }
}

/**
 * @brief Updates the progress bar in the progress dialog window.
 * 
 * @param hProgressBar Handle to the progress bar window.
 * @param progress The current progress value.
 */
void UpdateProgressBar(HWND hProgressBar, int progress) {
    if (hProgressBar != NULL) {
        SendMessage(hProgressBar, PBM_SETPOS, progress, 0);
    }
}

/**
 * @brief Window procedure for the progress dialog window.
 * 
 * @param hwndDlg Handle to the progress dialog window.
 * @param uMsg The message to be processed.
 * @param wParam Additional message-specific information.
 * @param lParam Additional message-specific information.
 * @return The result of the message processing and depends on the message type.
 */
LRESULT CALLBACK ProgressDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            CloseProgressDialog(hwndDlg);
            break;
        default:
            return DefWindowProc(hwndDlg, uMsg, wParam, lParam);
    }
    return 0;
}