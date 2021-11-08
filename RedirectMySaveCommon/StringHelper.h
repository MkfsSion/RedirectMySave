#pragma once

#include <string>

#include <Windows.h>

class StringHelper
{
public:
    static std::wstring toWideString(const std::string& str) noexcept;

    static std::string toString(const std::wstring& wstr) noexcept;

    static std::wstring toDOSPath(const std::wstring& unc) noexcept;
};

