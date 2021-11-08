#pragma once

#include "LoggerPolicy.h"
#include "StringHelper.h"

class MessageBoxLoggerPolicy
{
public:
    template <SupportedStringType T> void log(LoggerLevel level, T str) {
        using U = GetStringType<T>;
        U message = str;
        UINT logType;
        std::wstring logTitle;
        switch (level) {
        case LoggerLevel::Debug:
            logTitle = L"Debug";
            logType = MB_OK | MB_ICONINFORMATION;
            break;
        case LoggerLevel::Info:
            logTitle = L"Information";
            logType = MB_OK | MB_ICONINFORMATION;
            break;
        case LoggerLevel::Warn:
            logTitle = L"Warn";
            logType = MB_OK | MB_ICONWARNING;
            break;
        case LoggerLevel::Error:
            logTitle = L"Error";
            logType = MB_OK | MB_ICONERROR;
            break;
        case LoggerLevel::Fatal:
            logTitle = L"Fatal";
            logType = MB_OK | MB_ICONERROR;
        }
        if constexpr (std::same_as<U, std::string>)
            MessageBoxA(nullptr, message.c_str(), StringHelper::toString(logTitle).c_str(), logType);
        else
            MessageBoxW(nullptr, message.c_str(), logTitle.c_str(), logType);
    }
};
