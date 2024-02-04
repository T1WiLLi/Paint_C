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
#include <commctrl.h>
#include <stdio.h>

// Color ID
#define ID_COLOR_BLACK  101
#define ID_COLOR_RED    102
#define ID_COLOR_GREEN  103
#define ID_COLOR_BLUE   104
#define ID_COLOR_YELLOW 105
#define ID_COLOR_CYAN   106
#define ID_COLOR_MAGENTA 107
#define ID_COLOR_WHITE  108
#define ID_COLOR_ORANGE   109
#define ID_COLOR_PINK     110
#define ID_COLOR_PURPLE   111
#define ID_COLOR_BROWN    112
#define ID_COLOR_LIME     113
#define ID_COLOR_TURQUOISE 114
#define ID_COLOR_GOLDENROD 115
#define ID_COLOR_LAVENDER 116
#define ID_COLOR_CORAL 117
#define ID_COLOR_SEAGREEN 118

// Settings ID
#define ID_GRID_TOGGLE  119
#define ID_INCREASE_BRUSH 120
#define ID_DECREASE_BRUSH 121
#define ID_BRUSHSIZE_LABEL 122

// Other
#define ID_RESET 123

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
 * @brief Toggles the grid feature on or off.
 */
void ToggleGrid();
/**
 * @brief Writes a message to the debug log file.
 *
 * @param message The message to be written to the log.
 */
void LogToFile(const char* message);
/**
 * @brief Resets the canvas of the specified window.
 *
 * @param hwnd The handle to the window.
 */
void ResetCanvas(HWND hwnd);

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
BOOL useGrid = FALSE;
int brushSize = 1;

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
        TEXT("Paint Program | By William Beaudin | Version: 2024-02-03/2.2"),
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
    CreateWindow(TEXT("BUTTON"), TEXT("Yellow"), WS_VISIBLE | WS_CHILD, 10, 170, 80, 30, hwnd, (HMENU)ID_COLOR_YELLOW, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Cyan"), WS_VISIBLE | WS_CHILD, 10, 210, 80, 30, hwnd, (HMENU)ID_COLOR_CYAN, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Magenta"), WS_VISIBLE | WS_CHILD, 10, 250, 80, 30, hwnd, (HMENU)ID_COLOR_MAGENTA, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Orange"), WS_VISIBLE | WS_CHILD, 10, 290, 80, 30, hwnd, (HMENU)ID_COLOR_ORANGE, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Pink"), WS_VISIBLE | WS_CHILD, 10, 330, 80, 30, hwnd, (HMENU)ID_COLOR_PINK, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Purple"), WS_VISIBLE | WS_CHILD, 10, 370, 80, 30, hwnd, (HMENU)ID_COLOR_PURPLE, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Brown"), WS_VISIBLE | WS_CHILD, 10, 410, 80, 30, hwnd, (HMENU)ID_COLOR_BROWN, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Lime"), WS_VISIBLE | WS_CHILD, 10, 450, 80, 30, hwnd, (HMENU)ID_COLOR_LIME, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Turquoise"), WS_VISIBLE | WS_CHILD, 10, 490, 80, 30, hwnd, (HMENU)ID_COLOR_TURQUOISE, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Goldenrod"), WS_VISIBLE | WS_CHILD, 10, 530, 80, 30, hwnd, (HMENU)ID_COLOR_GOLDENROD, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Lavender"), WS_VISIBLE | WS_CHILD, 10, 570, 80, 30, hwnd, (HMENU)ID_COLOR_LAVENDER, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Coral"), WS_VISIBLE | WS_CHILD, 10, 610, 80, 30, hwnd, (HMENU)ID_COLOR_CORAL, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Seagreen"), WS_VISIBLE | WS_CHILD, 10, 650, 80, 30, hwnd, (HMENU)ID_COLOR_SEAGREEN, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("ERASER"), WS_VISIBLE | WS_CHILD, 100, 10, 80, 30, hwnd, (HMENU)ID_COLOR_WHITE, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Toggle Grid"), WS_VISIBLE | WS_CHILD, 190, 10, 100, 30, hwnd, (HMENU)ID_GRID_TOGGLE, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Brush+"), WS_VISIBLE | WS_CHILD, 300, 10, 80, 30, hwnd, (HMENU)ID_INCREASE_BRUSH, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Brush-"), WS_VISIBLE | WS_CHILD, 390, 10, 80, 30, hwnd, (HMENU)ID_DECREASE_BRUSH, hInstance, NULL);
    CreateWindow(TEXT("BUTTON"), TEXT("Reset"), WS_VISIBLE | WS_CHILD | SS_CENTER, 480, 10, 80, 30, hwnd, (HMENU) ID_RESET, hInstance, NULL);
    CreateWindow(TEXT("STATIC"), TEXT("Brush Size: 1"), WS_VISIBLE | WS_CHILD | SS_CENTER, 570, 15, 100, 20, hwnd, (HMENU) ID_BRUSHSIZE_LABEL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(colorBrush);
    DestroyIcon(hIcon);
    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static POINT prevPoint;

    switch (uMsg) {
    case WM_LBUTTONDOWN:
        prevPoint.x = LOWORD(lParam);
        prevPoint.y = HIWORD(lParam);
        DrawPixel(hwnd, prevPoint.x, prevPoint.y);
        break;

    case WM_LBUTTONUP:
        // Do nothing on button release
        break;

    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON) {
            POINT currentPoint;
            currentPoint.x = LOWORD(lParam);
            currentPoint.y = HIWORD(lParam);
            DrawPixel(hwnd, currentPoint.x, currentPoint.y);
            prevPoint = currentPoint;
        }
        break;

    case WM_COMMAND:
        // button clicks to update buttons and settings
        switch (LOWORD(wParam)) {
            case ID_COLOR_BLACK:
                SetColor(0, 0, 0);
                break;
            case ID_COLOR_RED:
                SetColor(255, 0, 0);
                break;
            case ID_COLOR_GREEN:
                SetColor(0, 255, 0);
                break;
            case ID_COLOR_BLUE:
                SetColor(0, 0, 255);
                break;
            case ID_COLOR_YELLOW:
                SetColor(255, 255, 0);
                break;
            case ID_COLOR_CYAN:
                SetColor(0, 255, 255);
                break;
            case ID_COLOR_MAGENTA:
                SetColor(255, 0, 255);
                break;
            case ID_COLOR_ORANGE:
                SetColor(255, 165, 0);
                break;
            case ID_COLOR_PINK:
                SetColor(255, 192, 203);
                break;
            case ID_COLOR_PURPLE:
                SetColor(128, 0, 128);
                break;
            case ID_COLOR_BROWN:
                SetColor(139, 69, 19);
                break;
            case ID_COLOR_LIME:
                SetColor(0, 255, 0);
                break;
            case ID_COLOR_TURQUOISE:
                SetColor(64, 224, 208);
                break;
            case ID_COLOR_GOLDENROD:
                SetColor(218, 165, 32);
                break;
            case ID_COLOR_LAVENDER:
                SetColor(230, 230, 250);
                break;
            case ID_COLOR_CORAL:
                SetColor(255, 127, 80);
                break;
            case ID_COLOR_SEAGREEN:
                SetColor(46, 139, 87);
                break;
            case ID_COLOR_WHITE:
                SetColor(255, 255, 255);
                break;
            case ID_GRID_TOGGLE:
                ToggleGrid();
                break;
            case ID_INCREASE_BRUSH:
                if (brushSize != 26) {
                    if (brushSize == 1) {
                        brushSize++;
                    } else {
                        brushSize += 2;
                    }
                }
                break;
            case ID_DECREASE_BRUSH:
                if(brushSize != 1) {
                    if (brushSize == 2) {
                        brushSize--;
                    } else {
                        brushSize -= 2;
                    }
                }
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
    SetWindowText(GetDlgItem(hwnd, ID_BRUSHSIZE_LABEL), buffer);
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

    int halfSize = brushSize / 2;

    for (int i = -halfSize; i <= halfSize; ++i) {
        for (int j = -halfSize; j <= halfSize; ++j) {
            SetPixel(hdc, x + i, y + j, RGB(GetRValue(lb.lbColor), GetGValue(lb.lbColor), GetBValue(lb.lbColor)));
        }   
    }

    ReleaseDC(hwnd, hdc);
}

void SetColor(int r, int g, int b) {
    DeleteObject(colorBrush);
    colorBrush = CreateSolidBrush(RGB(r, g, b));
}

void ToggleGrid() {
    useGrid = !useGrid;
}

void LogToFile(const char* message) {
    FILE* file = fopen("logfile.txt", "a");
    if (file != NULL) {
        fprintf(file, "DEBUG: %s\n", message);
        fclose(file);
    }
}
