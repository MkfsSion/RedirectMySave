#include "FunctionHelper.h"

#include <Windows.h>

void* FunctionHelper::getFunctionFromLib(const std::string& name, const std::string& libname) noexcept {
    HMODULE hm = GetModuleHandleA(libname.c_str());
    if (hm == nullptr)
        return nullptr;
    return GetProcAddress(hm, name.c_str());
}

std::wstring FunctionHelper::getWorkingDirectroy() noexcept {
    wchar_t buf[MAX_PATH] = { 0 };
    auto res = GetCurrentDirectoryW(MAX_PATH, buf);
    if (res <= 0)
        return L"";
    return std::wstring{ buf,  res };
}