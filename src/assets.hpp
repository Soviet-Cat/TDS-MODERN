#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

#include <wincodec.h>
#include <wincodecsdk.h>
#include <urlmon.h>
#include <d2d1.h>

#include "utils.hpp"

const std::string ASSET_URL = "https://raw.githubusercontent.com/Soviet-Cat/TDS-MODERN/main/assets/";
const std::string ASSET_PATH = APPDATA_PATH + "assets\\";
const std::string ASSET_DOWNLOADER_WORKER_STOP = "!STOP!";
const std::string ASSET_LOADER_WORKER_STOP = "!STOP!";

class App;
class App;

class AssetDownloader
{
    public:
    static void download(const std::string& name);
    static void worker();
    static void start();
    static void stop();
    static void push(const std::string& name);
    static float completion();

    static bool running;
    static std::thread thread;
    static std::queue<std::string> names;
    static std::mutex namesMutex;
    static std::condition_variable namesConditionVar;

    static float pushed;
    static float downloaded;
};

class AssetLoader
{
    public:
    ~AssetLoader();

    static void load(const std::string& name);
    static void worker();
    static void start();
    static void stop();
    static void push(const std::string& name);

    static ID2D1Bitmap* get(const std::string& name);

    static bool running;
    static std::unordered_map<std::string, ID2D1Bitmap*> assets;
    static std::thread thread;
    static std::queue<std::string> names;
    static std::mutex namesMutex;
    static std::condition_variable namesConditionVar;

    static ID2D1Factory* factory;
    static ID2D1HwndRenderTarget* renderTarget;
};
