#include <app.hpp>
#include <assets.hpp>
#include <iostream>
#include <filesystem>

#include "config.hpp"

int main()
{
    Config::load();

    App::createWindow();

    App::initializeDirectX();

    AssetDownloader::start();

    AssetDownloader::push("BG.png");
    AssetLoader::push("BG.png");

    bool running = true;
    while (running)
    {
        if (!App::processWindow())
        {
            running = false;
        }
        if (AssetDownloader::completion() == 100.0f)
        {
            AssetLoader::start();
        }
    }

    AssetLoader::stop();
    AssetDownloader::stop();

    return 0;
}
