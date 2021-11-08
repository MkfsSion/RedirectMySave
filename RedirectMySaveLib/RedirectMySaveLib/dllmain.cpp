#include <fstream>
#include <locale>

#include <Windows.h>
#include <winternl.h>

#include <FileLoggerPolicy.h>
#include <Logger.h>
#include <MessageBoxLoggerPolicy.h>
#include <RedirectMySaveConfig.h>
#include <WideStringException.h>

#include "FunctionHooker.h"

static Logger<FileLoggerPolicy>* fileLogger = nullptr;
static Logger<MessageBoxLoggerPolicy>* logger = nullptr;
static FunctionHooker* hooker = nullptr;
static RedirectMySaveConfig* config = nullptr;

NTSTATUS WINAPI NTCreateFile(
    PHANDLE            FileHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK   IoStatusBlock,
    PLARGE_INTEGER     AllocationSize,
    ULONG              FileAttributes,
    ULONG              ShareAccess,
    ULONG              CreateDisposition,
    ULONG              CreateOptions,
    PVOID              EaBuffer,
    ULONG              EaLength
)
{
    std::wstring str { ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length / sizeof(wchar_t) };
    if (fileLogger != nullptr) {
        fileLogger->debug("Enter NTCreateFile");
        fileLogger->debug(L"ObjectName:" +  str);
    }
    bool replace = false;
    std::wstring accessPath = L"";
    for (auto& it : config->mapping) {
        if (StringHelper::toDOSPath(str).find(it.first) == 0) {
            accessPath += L"\\??\\";
            accessPath += it.second;
            accessPath += str.substr(4 + it.first.length());
            fileLogger->debug(L"Found file " + str);
            fileLogger->debug(L"Access " + accessPath);
            replace = true;
            break;
        }
    }
    PUNICODE_STRING orig = ObjectAttributes->ObjectName;
    if (replace) {
        UNICODE_STRING us = { static_cast<USHORT>(accessPath.length() * sizeof(wchar_t)), static_cast<USHORT>(accessPath.size() * sizeof(wchar_t)) , &accessPath[0] };
        ObjectAttributes->ObjectName = &us;
    }
    NTSTATUS status = reinterpret_cast<decltype(&NTCreateFile)>(hooker->getFunction("NtCreateFile"))(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
    if(replace)
        ObjectAttributes->ObjectName = orig;
    return status;
}

NTSTATUS WINAPI NTOpenFile(
    PHANDLE            FileHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK   IoStatusBlock,
    ULONG              ShareAccess,
    ULONG              OpenOptions
)
{
    std::wstring str{ ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length / sizeof(wchar_t) };
    if (fileLogger != nullptr) {
        fileLogger->debug("Enter NTOpenFile");
        fileLogger->debug(L"ObjectName:" + str );
    }
    std::wstring accessPath = L"";
    bool replace = false;
    for (auto& it : config->mapping) {
        if (StringHelper::toDOSPath(str).find(it.first) == 0) {
            accessPath += L"\\??\\";
            accessPath += it.second;
            accessPath += str.substr(4 + it.first.length());
            fileLogger->debug(L"Found file " + str);
            fileLogger->debug(L"Access " + accessPath);
            replace = true;
            break;
        }
    }
    PUNICODE_STRING orig = ObjectAttributes->ObjectName;
    if (replace) {
        UNICODE_STRING us = { static_cast<USHORT>(accessPath.length() * sizeof(wchar_t)), static_cast<USHORT>(accessPath.size() * sizeof(wchar_t)) , &accessPath[0] };
        
        ObjectAttributes->ObjectName = &us;
    }
    NTSTATUS status = reinterpret_cast<decltype(&NTOpenFile)>(hooker->getFunction("NtOpenFile"))(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
    if (replace)
        ObjectAttributes->ObjectName = orig;
    return status;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  dwReason,
                       LPVOID lpReserved
                     )
{
    if (DetourIsHelperProcess())
        return TRUE;
    std::locale::global(std::locale(""));
    if (dwReason == DLL_PROCESS_ATTACH) {
        logger = new Logger<MessageBoxLoggerPolicy>(MessageBoxLoggerPolicy{});
        if (config == nullptr) {
            try {
                config = new RedirectMySaveConfig(FunctionHelper::getWorkingDirectroy() + L"\\RedirectMySave.json");
            }
            catch (std::invalid_argument& e) {
                logger->fatal(e.what());
                return FALSE;
            }
            catch (WideStringException& e) {
                logger->fatal(e.what());
                return FALSE;
            }
        }
        if (fileLogger == nullptr && config->debug) {
            try {
                fileLogger = new Logger<FileLoggerPolicy>{ FileLoggerPolicy { FunctionHelper::getWorkingDirectroy() + L"\\RedirectMySave.log" } };
            }
            catch (std::invalid_argument& e) {
                logger->fatal(e.what());
                return FALSE;
            }
            catch (WideStringException& e) {
                logger->fatal(e.what());
                return FALSE;
            }
            fileLogger->info("Start logging...");
        }
        DetourRestoreAfterWith();
        if (hooker == nullptr) {
            hooker = new FunctionHooker{};
            hooker->registerHook("NtCreateFile", "ntdll.dll", NTCreateFile);
            hooker->registerHook("NtOpenFile", "ntdll.dll", NTOpenFile);
        }
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        if (fileLogger != nullptr) {
            fileLogger->info("End logging...");
            delete fileLogger;
            fileLogger = nullptr;
        }

        if (logger != nullptr) {
            delete logger;
            logger = nullptr;
        }

        if (hooker != nullptr) {
            hooker->removeAllHook();
            delete hooker;
            hooker = nullptr;
        }
        if (config != nullptr) {
            delete config;
            config = nullptr;
        }
    }
    return TRUE;
}

