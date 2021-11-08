#include "StringHelper.h"

#include <locale>
#include <codecvt>

std::wstring StringHelper::toWideString(const std::string& str) noexcept {
    if (str.empty()) return std::wstring{};
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string StringHelper::toString(const std::wstring& wstr) noexcept {
    if (wstr.empty()) return std::string{};
    size_t length = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), nullptr, 0, nullptr, nullptr);
    std::string str (length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &str[0], str.length(), nullptr, nullptr);
    return str;
}

std::wstring StringHelper::toDOSPath(const std::wstring& unc) noexcept {
    size_t pos;
    std::wstring path = unc;
    if ((pos = unc.find(L"\\??\\")) != std::wstring::npos) {
        path = unc.substr(4);
    }
    return path;
}
