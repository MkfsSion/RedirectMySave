#pragma once

#include <concepts>
#include <string>

#include "ConceptCommon.h"
#include "LoggerPolicy.h"

template <LoggerPolicy P> class Logger {
public:
    Logger(const P& policy) : mPolicy(policy) { }
    Logger(P &&policy) : mPolicy(std::move(policy)) { }
    template <SupportedStringType T> void debug(T str) {
        mPolicy.log(LoggerLevel::Debug, str);
    }
    template <SupportedStringType T> void info(T str) {
        mPolicy.log(LoggerLevel::Info, str);
    }
    template <SupportedStringType T> void warn(T str) {
        mPolicy.log(LoggerLevel::Warn, str);
    }
    template <SupportedStringType T> void error(T str) {
        mPolicy.log(LoggerLevel::Error, str);
    }
    template <SupportedStringType T> void fatal(T str) {
        mPolicy.log(LoggerLevel::Fatal, str);
    }
private:
    P mPolicy;
};