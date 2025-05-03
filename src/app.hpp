#pragma once

#include <string>
#include <windows.h>
#include <d2d1.h>

#include "config.hpp"
#include "assets.hpp"

const std::string APP_WND_CLASS_NAME = "TDS_MODERN_WINDOW_CLASS";
const int APP_DISPLAY_WIDTH = 640;
const int APP_DISPLAY_HEIGHT = 360;

class App
{
    public:
    ~App();

    static void createWindow();
    static void destroyWindow();
    static bool processWindow();

    static void createDisplay();

    static void fullscreen();
    static void borderless();
    static void windowed();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static void initializeDirectX();

    private:
    static HWND hWnd;
    static HINSTANCE hInst;
    static WNDCLASSEX wndClass;

    static ID2D1Bitmap* display;
    static ID2D1Factory* factory;
    static ID2D1HwndRenderTarget* renderTarget;
};