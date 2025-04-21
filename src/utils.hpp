#pragma once

#include <string>
#include <windows.h>
#include <shlobj.h>

const std::string APPDATA_FOLDER = "\\TDS MODERN\\";

std::string getAppDataPath();

const std::string APPDATA_PATH = getAppDataPath();

std::string pathToUrl(const std::string& path);
