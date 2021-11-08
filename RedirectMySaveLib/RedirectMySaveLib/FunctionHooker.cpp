#include "FunctionHooker.h"

void FunctionHooker::removeHook(const std::string& name) {
    auto x = mInfos.find(name);
    if (x == mInfos.end())
        return;
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(reinterpret_cast<void**>(&x->second.func), reinterpret_cast<void*>(const_cast<void *>(x->second.hook)));
    DetourTransactionCommit();
    mInfos.erase(x);
}

void FunctionHooker::removeDetourHook(void** func, void* hook) {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    removeDetourHookUnsafe(func, hook);
    DetourTransactionCommit();
}
