#include "app.hpp"

HWND App::hWnd;
HINSTANCE App::hInst;
WNDCLASSEX App::wndClass;
ID2D1Factory* App::factory;
ID2D1HwndRenderTarget* App::renderTarget;

void App::initializeDirectX()
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
    RECT rect;
    GetClientRect(hWnd, &rect);
    D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);
    factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hWnd, size),
        &renderTarget);

    AssetLoader::factory = factory;
    AssetLoader::renderTarget = renderTarget;
}

void App::createWindow()
{
    hInst = GetModuleHandle(nullptr);

    wndClass.lpszClassName = APP_WND_CLASS_NAME.c_str();
    wndClass.hInstance = hInst;
    wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbSize = sizeof(wndClass);

    if (!RegisterClassEx(&wndClass))
    {
        std::cout << "RegisterClassEx failed" << std::endl;
    }

    DWORD wndStyle = WS_OVERLAPPEDWINDOW;

    RECT wndRect;

    wndRect.left = 0;
    wndRect.top = 0;
    wndRect.right = wndRect.left + Config::get<int>("App.windowWidth");
    wndRect.bottom = wndRect.top + Config::get<int>("App.windowHeight");

    AdjustWindowRect(&wndRect, wndStyle, false);

    hWnd = CreateWindowEx(
        0,
        APP_WND_CLASS_NAME.c_str(),
        "TDS MODERN",
        wndStyle,
        wndRect.left,
        wndRect.top,
        wndRect.right - wndRect.left,
        wndRect.bottom - wndRect.top,
        nullptr,
        nullptr,
        hInst,
        nullptr);

    if (!hWnd)
    {
        std::cout << "CreateWindowEx failed" << std::endl;
    }

    ShowWindow(hWnd, SW_SHOW);

    auto mode = Config::get<std::string>("App.windowMoe");
    if (mode == "windowed")
    {
        windowed();
    } else if (mode == "borderless")
    {
        borderless();
    }

    SetTimer(hWnd, 1, 1000 / 60, nullptr);
}

void App::destroyWindow()
{
    int result = MessageBox(hWnd, "Are you sure you want to quit TDS MODERN?", "Come back later!", MB_YESNO);
    if (result == IDYES)
    {
        DestroyWindow(hWnd);
        hWnd = nullptr;
    }
}

void App::windowed()
{
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &mi);

    int width = Config::get<int>("App.windowWidth");
    int height = Config::get<int>("App.windowHeight");

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    if (width < 0)
    {
        rect.right = mi.rcMonitor.right - mi.rcMonitor.left;
    } else
    {
        rect.right = rect.left + width;
    }
    if (height < 0)
    {
        rect.bottom = mi.rcMonitor.bottom - mi.rcMonitor.top;
    } else
    {
        rect.bottom = rect.top + height;
    }

    SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    SetWindowPos(hWnd, HWND_TOP,
        rect.left, rect.top,
        rect.right,
        rect.bottom,
        SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

    Config::set("App.windowMode", "windowed");
    Config::save();
}

void App::borderless()
{
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &mi);

    SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(hWnd, HWND_TOP,
        mi.rcMonitor.left, mi.rcMonitor.top,
        mi.rcMonitor.right - mi.rcMonitor.left,
        mi.rcMonitor.bottom - mi.rcMonitor.top,
        SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

    Config::set("App.windowMode", "borderless");
    Config::save();
}

LRESULT App::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
            {
                if (renderTarget)
                {
                    renderTarget->BeginDraw();
                    renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

                    ID2D1Bitmap* bitmap = nullptr;

                    float bitmapX = 100.0f;
                    float bitmapY = 100.0f;

                    try
                    {
                        bitmap = AssetLoader::get("BG.png");
                    } catch(...)
                    {

                    }

                    if (bitmap)
                    {
                        D2D1_SIZE_F bitmapSize = bitmap->GetSize();
                        renderTarget->DrawBitmap(bitmap, D2D1::RectF(bitmapX, bitmapY, bitmapX + bitmapSize.width, bitmapY + bitmapSize.height));
                    }

                    renderTarget->EndDraw();
                }

                ValidateRect(hWnd, nullptr);
                break;
            }
        case WM_TIMER:
            InvalidateRect(hWnd, nullptr, true);
            break;
        case WM_CLOSE:
            destroyWindow();
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_F11)
            {
                if (Config::get<std::string>("App.windowMode") == "windowed")
                {
                    borderless();
                } else if (Config::get<std::string>("App.windowMode") == "borderless")
                {
                    windowed();
                }
            }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool App::processWindow()
{
    MSG msg;

    while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

App::~App()
{
    if (factory)
    {
        factory->Release();
        factory = nullptr;
    }

    if (renderTarget)
    {
        renderTarget->Release();
        renderTarget = nullptr;
    }

    UnregisterClass(APP_WND_CLASS_NAME.c_str(), hInst);
}
