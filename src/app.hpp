#pragma once

#include <windows.h>

class App
{
    public:
    static void createWindow();
    static void destroyWindow();

    private:
    static HWND hwnd;
    static HINSTANCE hInst;
    static WNDCLASSEX wndClass;
};