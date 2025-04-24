#include "app.hpp"

DWORD App::wndStyle;
HWND App::hWnd;
RECT App::wndRect;
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
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbSize = sizeof(wndClass);

    if (!RegisterClassEx(&wndClass))
    {
        std::cout << "RegisterClassEx failed" << std::endl;
    }

    wndStyle = WS_OVERLAPPEDWINDOW;

    wndRect.left = 100;
    wndRect.top = 100;
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

LRESULT App::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
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
