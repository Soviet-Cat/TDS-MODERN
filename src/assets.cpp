#include "assets.hpp"

size_t Assets::WriteCallback(void* contents, size_t size, size_t nmemb, std::ofstream* userp)
{
    size_t total = size * nmemb;
    userp->write(static_cast<const char*>(contents), total);
    return total;
}

void Assets::download(const std::string& name)
{
    std::string path = APPDATA_PATH + name;
    CURL* curl;
    CURLcode res;

    std::filesystem::path parent = std::filesystem::path(path).parent_path();
    if (!exists(parent))
    {
        create_directories(parent);
    }

    std::ofstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Error opening asset file: " << name << "\n";
    }

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, ASSET_URL + pathToUrl(name));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    file.close();
}


