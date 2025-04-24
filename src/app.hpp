#pragma once

#include <string>
#include <windows.h>
#include <d2d1.h>

#include "config.hpp"
#include "assets.hpp"

const std::string APP_WND_CLASS_NAME = "TDS_MODERN_WINDOW_CLASS";

class App
{
    public:
    ~App();

    static void createWindow();
    static void destroyWindow();
    static bool processWindow();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static void initializeDirectX();

    private:
    static DWORD wndStyle;
    static HWND hWnd;
    static RECT wndRect;
    static HINSTANCE hInst;
    static WNDCLASSEX wndClass;

    static ID2D1Factory* factory;
    static ID2D1HwndRenderTarget* renderTarget;
};