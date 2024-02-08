/*
    Paint Program - Documentation

    Author: William Beaudin
    Version: 2024-02-03/2.2

    This paint program allows users to draw on a canvas with various colors, brush sizes, and grid options.

    Table of Contents:
    1. Global Definitions
    2. Function Prototypes
    3. WinMain - Program Entry Point
    4. Window Procedure (WindowProc) - Handles Window Messages
    5. ResetCanvas - Resets the Canvas Background
    6. DrawPixel - Draws a Pixel on the Canvas
    7. SetColor - Sets the Drawing Color
    8. ToggleGrid - Toggles the Grid Display
    9. LogToFile - Logs Debug Information to a File

*/

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <tchar.h>
#include <wingdi.h>

// Color ID
#define ID_COLOR_BLACK  101
#define ID_COLOR_RED  102
#define ID_COLOR_GREEN 103
#define ID_COLOR_BLUE 104

// Custom Color ID
#define ID_COLOR_LABEL 105
#define ID_CUSTOM_RGB 106
#define ID_CUSTOM_BUTTON_COLOR 107

// Settings ID
#define ID_GRID_TOGGLE  119
#define ID_BRUSH_SLIDER 120
#define MIN_BRUSH_SIZE 1
#define MAX_BRUSH_SIZE 26


// Drawing-mode
#define ID_ERASER 122
#define ID_LINE 123

// Other
#define ID_RESET 130
#define ID_SAVE_BUTTON 131
#define ID_LOAD_BUTTON 132
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Progress bar

#define IDD_PROGRESS_DIALOG 133
#define IDC_PROGRESS_BAR 134
#define IDC_PROGRESS_TEXT 135

// Struct def

typedef struct ColorMapping {
    int r;
    int g;
    int b;
    const char* name;
} ColorMapping;

// Color mapping

ColorMapping* colorTable = NULL; // Dynamic array of map
int colorCount = 0;

// Prototype

/**
 * @brief Draws a pixel on the specified window at the given coordinates.
 *
 * @param hwnd The handle to the window.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 */
void DrawPixel(HWND hwnd, int x, int y);
/**
 * @brief Sets the color used for drawing.
 *
 * @param r The red component of the color.
 * @param g The green component of the color.
 * @param b The blue component of the color.
 */
void SetColor(int r, int g, int b);
/**
 * @brief Writes a message to the debug log file.
 *
 * @param format The message to be written to the log.
 */
void LogToFile(const char* format, ...);
/**
 * @brief Resets the canvas of the specified window.
 *
 * @param hwnd The handle to the window.
 */
void ResetCanvas(HWND hwnd);

const char* GetClosestColorName(int r, int g, int b);
void UpdateStatusBarText();
const char* GetCurrentModeText();
void resetColorTextField();
void DrawCustomLine(HWND hwnd, int startX, int startY, int endX, int endY);
void capture_pixel_data(FILE *file);
void load_pixel_data(FILE *file);

LRESULT CALLBACK ProgressDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateProgressBar(int progress);
void CloseProgressDialog();
void ShowProgressDialog(HWND hwndParent);
void CreateProgressBar(HWND hwndParent);
void loadColorTableFromCSV(const char* filename);
char* trim(char *str);


/**
 * @brief Window procedure that handles messages for the main window.
 *
 * @param hwnd The handle to the window.
 * @param uMsg The message identifier.
 * @param wParam Additional message information.
 * @param lParam Additional message information.
 * @return The result of the message processing and depends on the message.
 */

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Global variables

HWND hwnd;
HBRUSH colorBrush; // Default color is black
HWND hBrushSlider;
HWND hProgressDialog = NULL;
HWND hProgressBar = NULL;
HWND hProgressText = NULL;
int currentColor[3] = {0,0,0};
HWND hStatusBar;
HCURSOR hCustomCursor;
BOOL useGrid = FALSE;
BOOL eraseMode = FALSE;
BOOL lineMode = FALSE;
int brushSize = 1;
FILE *file;

BOOL useCustomColor = FALSE;
int CustomColor[3] = {0,0,0}; // Default custom color is black

int mouseX = 0;
int mouseY = 0;

/**
 * @brief The main entry point for the application.
 *
 * @param hInstance The handle to the current instance of the application.
 * @param hPrevInstance The handle to the previous instance of the application.
 * @param lpCmdLine The command-line parameters.
 * @param nCmdShow Controls how the window is to be shown.
 * @return The exit code.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS windowClass = { 0 };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hbrBackground = CreateSolidBrush(RGB(255,255,255)); 
    windowClass.lpszClassName = TEXT("PaintWindowClass");
    colorBrush = CreateSolidBrush(RGB(0, 0, 0));

    if (!RegisterClass(&windowClass)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    

    hwnd = CreateWindowEx(
        0,
        TEXT("PaintWindowClass"),
        TEXT("Paint Program | By William Beaudin | Version: 2024-02-07/3.2"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, TEXT("Window Creation Failed!"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    // Load the Icon for the App
    HICON hIcon = (HICON)LoadImage(hInstance, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    // Set the icon for the window
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

    // Create buttons
    CreateWindow(TEXT("BUTTON"), TEXT("Black"), WS_VISIBLE | WS_CHILD, 10, 10, 80, 30, hwnd, (HMENU)ID_COLOR_BLACK, hInstance, NULL); // Pos is: x, y, width, height
    CreateWindow(TEXT("BUTTON"), TEXT("Red"), WS_VISIBLE | WS_CHILD, 10, 50, 80, 30, hwnd, (HMENU)ID_COLOR_RED, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Green"), WS_VISIBLE | WS_CHILD, 10, 90, 80, 30, hwnd, (HMENU)ID_COLOR_GREEN, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Blue"), WS_VISIBLE | WS_CHILD, 10, 130, 80, 30, hwnd, (HMENU)ID_COLOR_BLUE, hInstance, NULL);
    CreateWindow(TEXT("STATIC"), TEXT("Custom RGB Value"), WS_VISIBLE | WS_CHILD | SS_CENTER, 10, 170, 130, 20, hwnd, (HMENU) ID_CUSTOM_RGB, hInstance, NULL);
    HWND hEdit = CreateWindowEx(0, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 10, 200, 100, 20, hwnd, (HMENU)ID_COLOR_LABEL, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("CUSTOM COLOR"), WS_VISIBLE | WS_CHILD, 10, 230, 150, 30, hwnd, (HMENU)ID_CUSTOM_BUTTON_COLOR, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Eraser"), WS_VISIBLE | WS_CHILD, 100, 10, 80, 30, hwnd, (HMENU)ID_ERASER, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Toggle Grid"), WS_VISIBLE | WS_CHILD, 190, 10, 100, 30, hwnd, (HMENU)ID_GRID_TOGGLE, hInstance, NULL);
    hBrushSlider = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS, 300, 10, 200, 30, hwnd, (HMENU)ID_BRUSH_SLIDER, hInstance, NULL);
    SendMessage(hBrushSlider, TBM_SETRANGE, TRUE, MAKELPARAM(MIN_BRUSH_SIZE, MAX_BRUSH_SIZE));
    SendMessage(hBrushSlider, TBM_SETPOS, TRUE, brushSize);
    CreateWindow(TEXT("BUTTON"), TEXT("Reset"), WS_VISIBLE | WS_CHILD | SS_CENTER, 510, 10, 80, 30, hwnd, (HMENU) ID_RESET, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Line-Mode"), WS_VISIBLE | WS_CHILD | SS_CENTER, 600, 10, 80, 30, hwnd, (HMENU) ID_LINE, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("SAVE"), WS_VISIBLE | WS_CHILD | SS_CENTER, 690, 10, 80, 30, hwnd, (HMENU) ID_SAVE_BUTTON, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("LOAD"), WS_VISIBLE | WS_CHILD | SS_CENTER, 780, 10, 80, 30, hwnd, (HMENU) ID_LOAD_BUTTON, hInstance, NULL);

    hStatusBar = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        NULL, 
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0,0,0,0,
        hwnd, 
        NULL,
        hInstance,
        NULL
    );

    int parts[] = {80, 230, 350, 520, -1};
    SendMessage(hStatusBar, SB_SETPARTS, sizeof(parts) / sizeof(parts[0]), (LPARAM)parts);

    // Load colorTable data from CSV
    loadColorTableFromCSV("colormap.csv");

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    free(colorTable);
    DeleteObject(colorBrush);
    DestroyIcon(hIcon);
    DestroyCursor(hCustomCursor);
    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static POINT startPoint;
    static POINT prevPoint;

    // Load the default cross cursor
    hCustomCursor = LoadCursor(NULL, IDC_CROSS);
    if (hCustomCursor == NULL) {
        DWORD error = GetLastError();
        LogToFile("ERROR: Failed to load cursor. Error code: %lu\n", error);
    }

    // Set the cursor to the default cross cursor
    SetCursor(hCustomCursor);

    switch (uMsg) {
    case WM_LBUTTONDOWN:
        if (lineMode) {
            startPoint.x = LOWORD(lParam);
            startPoint.y = HIWORD(lParam);
        } else {
            prevPoint.x = LOWORD(lParam);
            prevPoint.y = HIWORD(lParam);
            DrawPixel(hwnd, prevPoint.x, prevPoint.y);
        }
        break;

    case WM_LBUTTONUP:
        if (lineMode) {
            POINT endPoint;
            endPoint.x = LOWORD(lParam);
            endPoint.y = HIWORD(lParam);
            DrawCustomLine(hwnd, startPoint.x, startPoint.y, endPoint.x, endPoint.y);
        }
        break;

    case WM_MOUSEMOVE:
        mouseX = GET_X_LPARAM(lParam);
        mouseY = GET_Y_LPARAM(lParam);

        if (wParam & MK_LBUTTON && !lineMode) {
            POINT currentPoint;
            currentPoint.x = LOWORD(lParam);
            currentPoint.y = HIWORD(lParam);
            DrawPixel(hwnd, currentPoint.x, currentPoint.y);
            prevPoint = currentPoint;
        }
        break;
    case WM_HSCROLL:
        if ((HWND)lParam == hBrushSlider) {
            int newPosition = SendMessage(hBrushSlider, TBM_GETPOS, 0, 0);
            int roundedPosition = (newPosition / 2) * 2; // Round to the nearest multiple of 2

            if (roundedPosition != brushSize) {
                brushSize = roundedPosition;
                SendMessage(hBrushSlider, TBM_SETPOS, TRUE, brushSize);
            }
        }
        break;
    case WM_COMMAND:

        if (LOWORD(wParam) == ID_COLOR_LABEL && HIWORD(wParam) == EN_CHANGE) {
            char buffer[12];
            GetWindowText(GetDlgItem(hwnd, ID_COLOR_LABEL), buffer, sizeof(buffer));
            int r,g,b;
            if (sscanf(buffer, "%d,%d,%d", &r, &g, &b) == 3) {
                CustomColor[0] = r;
                CustomColor[1] = g;
                CustomColor[2] = b;

                const char* closestColorName = GetClosestColorName(CustomColor[0], CustomColor[1], CustomColor[2]);
                SetWindowText(GetDlgItem(hwnd, ID_CUSTOM_BUTTON_COLOR), closestColorName);
                if (useCustomColor) {
                    SetColor(CustomColor[0],CustomColor[1],CustomColor[2]);
                }

                LogToFile("DEBUG: New RGB Value: %d, %d, %d\n", r,g,b);
            }
            LogToFile("DEBUG: Text value changed: %s\n", buffer);
        }

        // button clicks to update buttons and settings
        switch (LOWORD(wParam)) {
            case ID_COLOR_BLACK:
                SetColor(0, 0, 0);
                useCustomColor = FALSE;
                resetColorTextField();
                break;
            case ID_COLOR_RED:
                SetColor(255, 0, 0);
                useCustomColor = FALSE;
                resetColorTextField();
                break;
            case ID_COLOR_GREEN:
                SetColor(0, 255, 0);
                useCustomColor = FALSE;
                resetColorTextField();
                break;
            case ID_COLOR_BLUE:
                SetColor(0, 0, 255);
                useCustomColor = FALSE;
                resetColorTextField();
                break;
            case ID_CUSTOM_BUTTON_COLOR:
                useCustomColor = !useCustomColor;
                if (useCustomColor) {
                    SetColor(CustomColor[0],CustomColor[1],CustomColor[2]);
                }
                LogToFile("DEBUG: State of the custom button: %d\n", (useCustomColor == TRUE) ? 1 : 0);
                break;
                case ID_SAVE_BUTTON:
                    DWORD start = GetTickCount();
                    LogToFile("DEBUG: SAVING...\n");
                    file = fopen("pixel_data.csv", "w"); 
                    if (file == NULL) {
                        LogToFile("ERROR: Failed to open file for writing.\n");
                        break;
                    }
                    capture_pixel_data(file);
                    fclose(file);

                    DWORD end = GetTickCount();
                    DWORD duration = end - start;
                    LogToFile("DEBUG: SAVING DONE.\n");
                    char durationStr[256];
                    sprintf(durationStr, "Time taken for saving: %lu milliseconds\n", duration);
                    LogToFile(durationStr);
                    break;
                case ID_LOAD_BUTTON:
                    start = GetTickCount();
                    LogToFile("DEBUG: CURRENT_LOADING\n");
                    file = fopen("pixel_data.csv", "r");
                    if (file == NULL) {
                        LogToFile("ERROR: Failed to open file for reading.\n");
                        break;
                    }
                    load_pixel_data(file);
                    fclose(file);

                    end = GetTickCount();
                    duration = end - start;

                    LogToFile("DEBUG: LOADING FINISHED\n");
                    durationStr[256];
                    sprintf(durationStr, "Time taken for loading: %lu milliseconds\n", duration);
                    LogToFile(durationStr);
                    break;
            case ID_ERASER:
                eraseMode = !eraseMode;
                if (eraseMode) {
                    lineMode = FALSE;
                    SetColor(255, 255, 255);
                } else {
                    SetColor(currentColor[0], currentColor[1], currentColor[2]);
                }
                break;
            case ID_GRID_TOGGLE:
                useGrid = !useGrid;
                PlaySound(_T("./click.wav"), NULL, SND_FILENAME | SND_ASYNC);
                break;
            case ID_LINE: 
                lineMode = !lineMode;
                break;
            case ID_RESET:
                ResetCanvas(hwnd);
                break;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    char buffer[50];
    wsprintf(buffer, TEXT("Brush Size: %d"), brushSize);
    UpdateStatusBarText();
    return 0;
}

void ResetCanvas(HWND hwnd) {
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255,255,255));
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)whiteBrush);
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
    DeleteObject(whiteBrush);
}

void DrawPixel(HWND hwnd, int x, int y) {
    HDC hdc = GetDC(hwnd);

    if (useGrid) {
        int gridSize = brushSize;
        x = ((x + gridSize / 2) / gridSize) * gridSize;
        y = ((y + gridSize / 2) / gridSize) * gridSize;
    }

    LOGBRUSH lb;
    GetObject(colorBrush, sizeof(LOGBRUSH), &lb);

    if (!eraseMode) {
        currentColor[0] = GetRValue(lb.lbColor);
        currentColor[1] = GetGValue(lb.lbColor);
        currentColor[2] = GetBValue(lb.lbColor);
    }

    int halfSize = brushSize / 2;

    for (int i = -halfSize; i <= halfSize; ++i) {
        for (int j = -halfSize; j <= halfSize; ++j) {
            SetPixel(hdc, x + i, y + j, RGB(GetRValue(lb.lbColor), GetGValue(lb.lbColor), GetBValue(lb.lbColor)));
        }   
    }

    ReleaseDC(hwnd, hdc);
}

void DrawCustomLine(HWND hwnd, int startX, int startY, int endX, int endY) {
    HDC hdc = GetDC(hwnd);

    if (useGrid) {
        int gridSize = brushSize;
        startX = ((startX + gridSize / 2) / gridSize) * gridSize;
        startY = ((startY + gridSize / 2) / gridSize) * gridSize;
        endX = ((endX + gridSize / 2) / gridSize) * gridSize;
        endY = ((endY + gridSize / 2) / gridSize) * gridSize;
    }

    int halfSize = brushSize / 2;

    for (int i = -halfSize; i <= halfSize; ++i) {
        for (int j = -halfSize; j <= halfSize; ++j) {
            int rectStartX = startX + i;
            int rectStartY = startY + j;
            int rectEndX = endX + i;
            int rectEndY = endY + j;

            LOGBRUSH lb;
            GetObject(colorBrush, sizeof(LOGBRUSH), &lb);
            
            if (!eraseMode) {
                currentColor[0] = GetRValue(lb.lbColor);
                currentColor[1] = GetGValue(lb.lbColor);
                currentColor[2] = GetBValue(lb.lbColor);
            }

            // Draw small rectangles between the starting and ending points with currentColor
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(currentColor[0], currentColor[1], currentColor[2]));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

            MoveToEx(hdc, rectStartX, rectStartY, NULL);
            LineTo(hdc, rectEndX, rectEndY);

            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
        }
    }

    ReleaseDC(hwnd, hdc);
}

void SetColor(int r, int g, int b) {
    DeleteObject(colorBrush);
    colorBrush = CreateSolidBrush(RGB(r, g, b));
}

void resetColorTextField() {
    SetWindowText(GetDlgItem(hwnd, ID_COLOR_LABEL), "");
    SetWindowText(GetDlgItem(hwnd, ID_CUSTOM_BUTTON_COLOR), "CUSTOM COLOR");
}

const char* GetClosestColorName(int r, int g, int b) {
    const ColorMapping* closestMatch = NULL;
    double minDistance = 1.79769e+308; // Maximum representable finite floating-point value

    for (int i = 0; i < colorCount; ++i) {
        const ColorMapping* mapping = &colorTable[i];
        double distance = sqrt(pow(mapping-> r - r, 2) + pow(mapping-> g - g, 2) + pow(mapping-> b - b, 2));

        if (distance < minDistance) {
            minDistance = distance;
            closestMatch = mapping;
        }
    }

    return (closestMatch != NULL) ? closestMatch-> name : colorTable[0].name; // Returning the first color as a fallback
}


void UpdateStatusBarText() {
    char brushSizeText[256];
    char colorRGBValueText[256];
    char currentModeText[256];
    char currentMousePositionText[256];
    sprintf_s(brushSizeText, sizeof(brushSizeText), "Brush Size: %d", brushSize);
    sprintf_s(colorRGBValueText, sizeof(colorRGBValueText), "Color: RGB(%d, %d, %d)", currentColor[0], currentColor[1], currentColor[2]);
    sprintf_s(currentModeText, sizeof(currentModeText), "Current Mode: %s", GetCurrentModeText());
    sprintf_s(currentMousePositionText, sizeof(currentMousePositionText), "Mouse Pos = X: %d, Y: %d", mouseX, mouseY);

    SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)brushSizeText);
    SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM)colorRGBValueText);
    SendMessage(hStatusBar, SB_SETTEXT, 2, (LPARAM)currentModeText);
    SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM)currentMousePositionText);
}

const char* GetCurrentModeText() {
    if (eraseMode) {
        return "ERASE";
    } else if (useGrid) {
        return "GRID";
    } else if (lineMode) {
        return "LINE";
    } else {
        return "FREE";
    }
}

void capture_pixel_data(FILE *file) {
    ShowProgressDialog(hwnd); // Show the progress dialog
    HDC hdc = GetDC(hwnd); // Get device context of the window
    if (hdc == NULL) {
        return;
    }

    RECT rect;
    GetClientRect(hwnd, &rect); // Get the dimensions of the window client area

    int totalPixels = (rect.right - rect.left) * (rect.bottom - rect.top);
    int pixelProcessed = 0;
    const int updateInterval = totalPixels / 100;

    // Create a memory device context and a compatible bitmap
    HDC memDC = CreateCompatibleDC(hdc);
    if (memDC == NULL) {
        ReleaseDC(hwnd, hdc);
        return;
    }
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
    if (hBitmap == NULL) {
        DeleteDC(memDC);
        ReleaseDC(hwnd, hdc);
        return;
    }
    HGDIOBJ hOldBitmap = SelectObject(memDC, hBitmap);

    // Copy the window pixels to the memory device context
    if (!BitBlt(memDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, 0, 0, SRCCOPY)) {
        SelectObject(memDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        ReleaseDC(hwnd, hdc);
        return;
    } 

    // Process pixel data
    for (int y = 0; y < rect.bottom - rect.top; y++) {
        for (int x = 0; x < rect.right - rect.left; x++) {
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
                UpdateProgressBar(progress);
            }
        }
    }
    CloseProgressDialog();

    // Clean up resources
    SelectObject(memDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(hwnd, hdc);
}


void load_pixel_data(FILE *file) {
    LogToFile("DEBUG: INIT LOADING...\n");
    rewind(file);

    HDC hdc = GetDC(hwnd); // Get device context of the window
    if (hdc == NULL) {
        return;
    }

    size_t buffer_size = 64;
    char *buffer = malloc(buffer_size * sizeof(char));

    while(fgets(buffer, buffer_size, file) != NULL) {
        int x, y, r, g, b;
        if (sscanf(buffer, "%d,%d,%d,%d,%d", &x, &y, &r, &g, &b) == 5) {
            if (r != 255 || g != 255 || b != 255) {
                SetPixel(hdc, x, y, RGB(r,g,b));
            }
        }
    }

    // Release device context
    free(buffer);
    ReleaseDC(hwnd, hdc);
}

void LogToFile(const char* format, ...) {
    FILE* debugFile = fopen("logfile.txt", "a");
    if (debugFile != NULL) {
        va_list args;
        va_start(args, format);
        vfprintf(debugFile, format, args);
        va_end(args);
        fclose(debugFile);
    }
}

// Function to create and initialize the progress bar
void CreateProgressBar(HWND hwndParent) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);

    hProgressBar = CreateWindowEx(
        0,
        PROGRESS_CLASS,
        NULL,
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
        10, 10, 200, 20,
        hwndParent,
        NULL,
        NULL,
        NULL
    );
}

void ShowProgressDialog(HWND hwndParent) {
    // Create the progress dialog
    hProgressDialog = CreateWindowEx(WS_EX_DLGMODALFRAME,
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
        hProgressBar = CreateWindowEx(0,
                                      PROGRESS_CLASS,
                                      NULL,
                                      WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                                      5, 5, 280, 20,
                                      hProgressDialog,
                                      (HMENU)IDC_PROGRESS_BAR,
                                      NULL,
                                      NULL);

        // Show the dialog
        ShowWindow(hProgressDialog, SW_SHOW);
    } else {
        LogToFile("FAILURE: THE SAVE-WINDOWS COULDN'T BE CREATED AND IT RESULTED IN AN AUTOMATIC FAILURE.");
    }
}

void CloseProgressDialog() {
    if (hProgressDialog != NULL) {
        DestroyWindow(hProgressDialog);
        hProgressDialog = NULL;
    }
}

void UpdateProgressBar(int progress) {
    if (hProgressBar != NULL) {
        SendMessage(hProgressBar, PBM_SETPOS, progress, 0);
    }
}

LRESULT CALLBACK ProgressDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            CloseProgressDialog();
            break;
        default:
            return DefWindowProc(hwndDlg, uMsg, wParam, lParam);
    }
    return 0;
}

void loadColorTableFromCSV(const char* filename) {
    LogToFile("\nDEBUG: STARTING THE LOADING OF COLORS\n");
    FILE* colorMapFile = fopen(filename, "r"); // Open in read-only
    if (colorMapFile == NULL) {
        LogToFile("ERROR: Failed to open the file.");
        return;
    }   

    char line[256];

    while(fgets(line, sizeof(line), colorMapFile)) {
        ColorMapping color;
        char* token = strtok(line, ",");
        if (token == NULL) continue;
            color.r = atoi(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
            color.g = atoi(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
            color.b = atoi(token);

        token = strtok(NULL, "\n");
        if (token == NULL) continue;
            color.name = strdup(trim(token));
        
        colorTable = realloc(colorTable, sizeof(ColorMapping) * (colorCount + 1));
        colorTable[colorCount++] = color;
    }
    fclose(colorMapFile);
}

char* trim(char *str) { // My Own implementation of the TRIM() function
    while(isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    char *end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    return str;
}