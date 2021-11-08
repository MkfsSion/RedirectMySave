#pragma once

#include <cassert>
#include <unordered_map>
#include <string>
#include <type_traits>

#include <RedirectMySaveDetours.h>

#include "FunctionHelper.h"

namespace {

    template <typename F> concept Function = std::is_function_v<F>;

    struct FunctionInfo {
        std::string libname;
        void* func;
        const void* hook;
    };
}

class FunctionHooker
{
public:

    template <Function F> void registerHook(const std::string& name, const std::string& libname, F *func, F* hook) {
        if (func == nullptr || hook == nullptr) {
            assert("Invalid arguments detected in registerHook");
            return;
        }
        mInfos.insert({ name, {libname, func, hook} });
        const auto &x = mInfos.find(name);
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(reinterpret_cast<void**>(&x->second.func), reinterpret_cast<void*>(hook));
        DetourTransactionCommit();
    }

    template <Function F> void registerHook(const std::string& name, const std::string& libname, F* hook) {
        F* func = reinterpret_cast<F*>(FunctionHelper::getFunctionFromLib(name, libname));
        if (func == nullptr) {
            assert(("Failed to get function " + name + " from " + libname).c_str());
            return;
        }
        return registerHook(name, libname, func, hook);
    }
    template <Function F> void registerHook(const std::string& name, F* func, F* hook) {
        return registerHook(name, "", func, hook);
    }

    void removeHook(const std::string& name);
    void removeAllHook() {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        for (auto& x : mInfos) {
            removeDetourHookUnsafe(&x.second.func, const_cast<void*>(x.second.hook));
        }
        DetourTransactionCommit();
        mInfos.clear();
    }
    void *getFunction(const std::string& name) {
        auto x = mInfos.find(name);
        return x == mInfos.end() ? nullptr : reinterpret_cast<void*>(x->second.func);

    }
    ~FunctionHooker() {
        if (!mInfos.empty()) {
            removeAllHook();
        }
    }
private:
    void removeDetourHookUnsafe(void** func, void* hook) {
        DetourDetach(func, hook);
    }
    void removeDetourHook(void **func, void *hook);
    std::unordered_map<std::string, FunctionInfo> mInfos;
};

