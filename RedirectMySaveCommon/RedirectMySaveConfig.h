#pragma once

#include "ConceptCommon.h"
#include "WideStringException.h"
#include "StringHelper.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <unordered_map>
class RedirectMySaveConfig
{
public:
    bool debug;
    std::wstring program;
    std::unordered_map<std::wstring, std::wstring> mapping;

    template <SupportedStringType T>  RedirectMySaveConfig(T path) {
        std::ifstream ifs{ path, std::ios_base::in };
        if (!ifs.is_open()) {
            if constexpr (std::same_as<T, std::string>)
                throw std::invalid_argument(std::string{ "Failed to open " } + path);
            else
                throw WideStringException(std::wstring{ L"Failed to open " } + path);
        }
        std::string jsonString{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
        nlohmann::json json;
        try {
            json = nlohmann::json::parse(jsonString);
        }
        catch (nlohmann::json::parse_error& err) {
            throw std::invalid_argument(err.what());
        }
        try {
            debug = json["debug"].get<bool>();
            program = StringHelper::toWideString(json["program"].get<std::string>());
            auto arr = json["pathMapping"];
            if (arr.empty())
                throw std::invalid_argument("No path mapping defined in config");
            for (auto& x : arr) {
                mapping.insert({ StringHelper::toWideString(x["source"].get<std::string>()), StringHelper::toWideString(x["target"].get<std::string>()) });
            }
        }
        catch (nlohmann::json::type_error& err) {
            throw std::invalid_argument(err.what());
        }
    }
};