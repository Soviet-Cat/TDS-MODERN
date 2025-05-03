#include "config.hpp"

std::unordered_map<std::string, std::string> Config::data = {};
const std::string Config::path = APPDATA_PATH + "config.ini";

void Config::load()
{
    std::cout << "Loading config: " << path << std::endl;
    std::ifstream ifs(path);
    if (ifs.is_open())
    {
        std::string line;
        std::string section;

        while (std::getline(ifs, line))
        {
            line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;
            if (line[0] == '[' && line.back() == ']')
            {
                section = line.substr(1, line.size() - 2);
            }
            size_t pos = line.find('=');
            if (pos != std::string::npos)
            {
                std::string key = section + "." + line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                data[key] = value;
            }
        }

        ifs.close();
    } else
    {
        std::cout << "Loading config failed: " << path << std::endl;
        std::cout << "Loading config defaults" << std::endl;
        defaults();
    }
}

void Config::save()
{
    std::filesystem::path appDataFolder = std::filesystem::path(path).parent_path();
    if (!exists(appDataFolder))
    {
        create_directory(appDataFolder);
    }

    std::cout << "Saving config: " << path << std::endl;

    std::ofstream ofs(path);
    if (!ofs.is_open())
    {
        std::cerr << "Saving config failed: " << path << std::endl;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> config;

    for (const auto& pair : data)
    {
        size_t pos = pair.first.find('.');
        if (pos != std::string::npos)
        {
            std::string section = pair.first.substr(0, pos);
            std::string key = pair.first.substr(pos + 1);
            config[section][key] = pair.second;
        }
    }

    for (const auto& section : config)
    {
        ofs << '[' << section.first << ']' << std::endl;
        for (const auto& pair : section.second)
        {
            ofs << pair.first << '=' << pair.second << std::endl;
        }
        ofs << std::endl;
    }

    ofs.close();
}

void Config::defaults()
{
    set("App.windowWidth", 640);
    set("App.windowHeight", 360);
    set("App.windowMode", "borderless");
    save();
}

void Config::print()
{
    std::cout << "Config:" << std::endl;
    for (const auto& pair : data) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}


