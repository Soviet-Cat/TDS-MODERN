#include "utils.hpp"

std::string getAppDataPath()
{
    wchar_t* path = nullptr;
    HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);
    if (SUCCEEDED(result))
    {
        std::wstring wstr(path);
        std::string str(wstr.begin(), wstr.end());
        str += APPDATA_FOLDER;
        return str;
    } else
    {
        return "";
    }
}

std::string pathToUrl(const std::string& path)
{
    std::string url = path;
    for (char& c : url)
    {
        if (c == '\\')
        {
            c = '/';
        }
    }
    return url;
}


