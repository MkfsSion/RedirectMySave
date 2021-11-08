#include <iostream>
#include <memory>

#include <RedirectMySaveDetours.h>
#include <RedirectMySaveConfig.h>
#include <FunctionHelper.h>
#include <WideStringException.h>

int main()
{
    std::locale::global(std::locale(""));
    std::shared_ptr<RedirectMySaveConfig> config;
    try {
        config = std::make_shared<RedirectMySaveConfig>(FunctionHelper::getWorkingDirectroy() + L"\\RedirectMySave.json");
    }
    catch (std::invalid_argument& e) {
        return -1;
    }
    catch (WideStringException& e) { return -1; }
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    std::wcout << config->program << std::endl;
    const char* libs[] = {
        "RedirectMySave.dll"
    };
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    int ret = DetourCreateProcessWithDlls(&config->program[0],
                                          &config->program[0],
                                          nullptr, nullptr,
                                          FALSE,
                                          NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE
                                          | CREATE_NEW_PROCESS_GROUP,
                                          nullptr, nullptr, &si,
                                          &pi, 1, libs, nullptr);
    if (!ret)
        return -1;
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}