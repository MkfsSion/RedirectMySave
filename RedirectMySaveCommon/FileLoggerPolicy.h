#pragma once

#include <iostream>
#include <fstream>

#include "ConceptCommon.h"
#include "LoggerPolicy.h"
#include "StringHelper.h"
#include "WideStringException.h"

class FileLoggerPolicy
{
public:
    FileLoggerPolicy(FileLoggerPolicy &&policy) noexcept {
        ofs = std::move(policy.ofs);
    }

    template <SupportedStringType T> FileLoggerPolicy(T path) {
        ofs.open(path, std::ios_base::out | std::ios_base::trunc);
        if (!ofs.is_open()) {
            if constexpr (std::same_as<GetStringType<T>, std::string>)
                throw std::invalid_argument(std::string{ "Failed to open log file " } + path);
            else
                throw WideStringException(std::wstring{ L"Failed to open log file " } + path);
        }
    }

    template <SupportedStringType T> void log(LoggerLevel level, T message) {
        std::wstring str = L"";
        using U = GetStringType<T>;
        switch (level) {
        case LoggerLevel::Debug:
            str += L"Debug: ";
            break;
        case LoggerLevel::Info:
            str += L"Information: ";
            break;
        case LoggerLevel::Warn:
            str += L"Warning: ";
            break;
        case LoggerLevel::Error:
            str += L"Error: ";
        case LoggerLevel::Fatal:
            str += L"Fatal: ";
        }
        if constexpr (!StandardStringType<std::decay_t<T>>) {
            if constexpr (std::same_as<std::decay_t<U>, std::string>)
                str += StringHelper::toWideString(U { message });
            else
                str += U{ message };
        }
        else if constexpr (std::same_as<std::decay_t<T>, std::string>)
            str += StringHelper::toWideString(message);
        else
            str += message;
        ofs << str << std::endl;
    }

    ~FileLoggerPolicy() {
        ofs.close();
    }
private:
    std::wofstream ofs;
};
