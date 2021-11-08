#pragma once

#include <string>

class WideStringException {
public:
    WideStringException() noexcept : _what(L"") { };
    WideStringException(const std::wstring& what) noexcept : _what(what) { };
    virtual ~WideStringException() noexcept { }
    virtual const std::wstring& what() noexcept { return _what; }
private:
    const std::wstring _what;
};