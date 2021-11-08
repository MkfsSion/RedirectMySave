#pragma once

#include <string>

#include "ConceptCommon.h"

enum class LoggerLevel {
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

template <typename T> concept LoggerPolicy = requires(T t, LoggerLevel level, const std::string & nstr, const std::wstring & wstr, const char* ptr, const wchar_t* wptr) {
    { t.log(level, nstr) } -> std::same_as<void>;
    { t.log(level, wstr) } -> std::same_as<void>;
    { t.log(level, ptr) } -> std::same_as<void>;
    { t.log(level, wptr) } -> std::same_as<void>;
}&& std::is_move_constructible_v<T>;