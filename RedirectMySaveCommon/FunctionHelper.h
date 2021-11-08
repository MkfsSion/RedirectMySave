#pragma once

#include <string>

class FunctionHelper
{
public:
    static void* getFunctionFromLib(const std::string& name, const std::string& libname) noexcept ;
    static std::wstring getWorkingDirectroy() noexcept;
};
