#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <list>

#include "utils.hpp"
#include "curl.h"

const std::string ASSET_URL = "https://github.com//Soviet-Cat/TDS-MODERN/tree/main/assets/";

class Assets
{
    public:
    static void download(const std::string& name);
    static void process();

    static std::list<std::string> names;
    private:
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::ofstream* userp);
};
