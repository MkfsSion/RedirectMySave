#pragma once

#include <concepts>
#include <string>

template <typename T> concept StandardStringType = std::same_as<std::decay_t<T>, std::string>
    || std::same_as<std::decay_t<T>, std::wstring>;

template <typename T> concept SupportedStringType = std::same_as<std::decay_t<T>, char*>
    || std::same_as<std::decay_t<T>, const char*>
        || std::same_as<std::decay_t<T>, wchar_t*>
        || std::same_as<std::decay_t<T>, const wchar_t*>
        || StandardStringType<T>;

namespace {
    namespace details {
        template <typename T> struct GetStringTypeImpl {
            using type = T;
        };

        template <> struct GetStringTypeImpl<char*> {
            using type = std::string;
        };

        template <> struct GetStringTypeImpl<const char*> {
            using type = std::string;
        };

        template <> struct GetStringTypeImpl<wchar_t*> {
            using type = std::wstring;
        };

        template <> struct GetStringTypeImpl<const wchar_t*> {
            using type = std::wstring;
        };
    }
}
template <SupportedStringType T> using GetStringType = details::GetStringTypeImpl<std::decay_t<T>>::type;