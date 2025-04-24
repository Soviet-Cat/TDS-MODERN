#include "assets.hpp"

std::condition_variable AssetDownloader::namesConditionVar;
std::mutex AssetDownloader::namesMutex;
std::thread AssetDownloader::thread;
std::queue<std::string> AssetDownloader::names;
bool AssetDownloader::running;

float AssetDownloader::downloaded;
float AssetDownloader::pushed;

std::condition_variable AssetLoader::namesConditionVar;
std::mutex AssetLoader::namesMutex;
std::thread AssetLoader::thread;
std::queue<std::string> AssetLoader::names;
std::unordered_map<std::string, ID2D1Bitmap*> AssetLoader::assets;
bool AssetLoader::running;
ID2D1Factory* AssetLoader::factory;
ID2D1HwndRenderTarget* AssetLoader::renderTarget;

void AssetDownloader::download(const std::string& name)
{
    std::string url = ASSET_URL + pathToUrl(name);
    std::string path = ASSET_PATH + name;

    std::filesystem::path savePath(path);
    if (!exists(savePath))
    {
        std::cout << "Downloading file: " << savePath.string() << std::endl;

        if (!exists(savePath.parent_path()))
        {
            create_directories(savePath.parent_path());
        }

        HRESULT result = URLDownloadToFile(nullptr, url.c_str(), path.c_str(), 0, nullptr);
        if (FAILED(result))
        {
            std::cout << "Download failed: " << result << std::endl;
        } else
        {
            downloaded++;
        }
    } else
    {
        pushed--;
    }
}

void AssetDownloader::worker()
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    while (true)
    {
        std::string name;

        {
            std::unique_lock lock(namesMutex);
            namesConditionVar.wait(lock, [] { return !names.empty(); });

            name = names.front();
            names.pop();
        }

        if (name == ASSET_DOWNLOADER_WORKER_STOP)
        {
            std::cout << "AssetDownloader worker stopping" << std::endl;
            break;
        }

        download(name);
    }

    CoUninitialize();
}

void AssetDownloader::push(const std::string& name)
{
    std::lock_guard lock(namesMutex);
    names.push(name);
    namesConditionVar.notify_one();
    pushed++;
}

void AssetDownloader::start()
{
    if (!running)
    {
        running = true;
        thread = std::thread(&worker);
        std::cout << "AssetDownloader worker starting" << std::endl;
    }
}

void AssetDownloader::stop()
{
    if (thread.joinable())
    {
        push(ASSET_DOWNLOADER_WORKER_STOP);
        thread.join();
        running = false;
    }
}

float AssetDownloader::completion()
{
    if (pushed == 0)
    {
        return 100.0f;
    }
    return (downloaded / pushed) * 100.0f;
}

void AssetLoader::load(const std::string& name)
{
    std::string path = ASSET_PATH + name;

    int size = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), static_cast<int>(path.size()), nullptr, 0);
    std::wstring wPath(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), static_cast<int>(path.size()), &wPath[0], size);

    std::filesystem::path savePath(path);
    if (exists(savePath))
    {
        std::cout << "Loading bitmap: " << name << std::endl;

        IWICImagingFactory* wicFactory = nullptr;
        IWICBitmapDecoder* decoder = nullptr;
        IWICBitmapFrameDecode* frameDecode = nullptr;
        IWICFormatConverter* converter = nullptr;

        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
        wicFactory->CreateDecoderFromFilename(wPath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
        decoder->GetFrame(0, &frameDecode);
        wicFactory->CreateFormatConverter(&converter);
        converter->Initialize(frameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom);

        ID2D1Bitmap* bitmap = nullptr;
        renderTarget->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);

        if (bitmap)
        {
            assets.emplace(name, bitmap);
        } else
        {
            std::cout << "Bitmap load failed: " << path << std::endl;
        }

        frameDecode->Release();
        decoder->Release();
        converter->Release();
        wicFactory->Release();
    }
}

ID2D1Bitmap* AssetLoader::get(const std::string& name)
{
    std::lock_guard lock(namesMutex);
    return assets.at(name);
}

void AssetLoader::worker()
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    while (true)
    {
        std::string name;

        {
            std::unique_lock lock(namesMutex);
            namesConditionVar.wait(lock, [] { return !names.empty(); });

            name = names.front();
            names.pop();
        }

        if (name == ASSET_LOADER_WORKER_STOP)
        {
            std::cout << "AssetLoader worker stopping" << std::endl;
            break;
        }

        load(name);
    }

    CoUninitialize();
}

void AssetLoader::push(const std::string& name)
{
    std::lock_guard lock(namesMutex);
    names.push(name);
    namesConditionVar.notify_one();
}

void AssetLoader::start()
{
    if (!running)
    {
        running = true;
        thread = std::thread(&worker);
        std::cout << "AssetLoader worker starting" << std::endl;
    }
}

void AssetLoader::stop()
{
    if (thread.joinable())
    {
        push(ASSET_LOADER_WORKER_STOP);
        thread.join();
        running = false;
    }
}

AssetLoader::~AssetLoader()
{
    for (auto asset: assets)
    {
        if (asset.second)
        {
            asset.second->Release();
        }
    }
    assets.clear();
}
