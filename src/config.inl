#ifndef CONFIG_INL
#define CONFIG_INL

#include "config.hpp"

template<typename T>
void Config::set(const std::string& key, const T& value)
{
    std::ostringstream oss;
    oss << value;
    data[key] = oss.str();
}

template<typename T>
T Config::get(const std::string& key)
{
    if (auto it = data.find(key); it != data.end())
    {
        std::istringstream iss(it->second);
        T value;
        iss >> value;
        return value;
    }
    return T();
}

#endif
