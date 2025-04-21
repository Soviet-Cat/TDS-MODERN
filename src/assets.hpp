#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "curl.h"

class Assets
{
    private:
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::ofstream* userp);
};
