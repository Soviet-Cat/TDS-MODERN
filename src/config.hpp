#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <unordered_map>

#include "utils.hpp"

class Config
{
    public:
    static void load();
    static void save();

    static void defaults();

    template<typename T>
    static void set(const std::string& key, const T& value);

    template<typename T>
    static T get(const std::string& key);

    static void print();

    private:

    static const std::string path;
    static std::unordered_map<std::string, std::string> data;
};

#include "config.inl"
