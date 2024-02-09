#include "LeagueOverlay.h"
#include <tlhelp32.h>
#include <iostream>


DWORD FindProcessID(const std::wstring& processName) {
    PROCESSENTRY32W processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32FirstW(processesSnapshot, &processInfo)) {
        do {
            if (std::wstring(processInfo.szExeFile) == processName) {
                CloseHandle(processesSnapshot);
                return processInfo.th32ProcessID;
            }
        } while (Process32NextW(processesSnapshot, &processInfo));
    }

    CloseHandle(processesSnapshot);
    return 0;
}

LPVOID AllocateMemory(HANDLE process, size_t size) {
    return VirtualAllocEx(process, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void FreeMemory(HANDLE process, LPVOID ptr) {
    VirtualFreeEx(process, ptr, 0, MEM_RELEASE);
}

std::vector<DWORD> GetThreadIDs(DWORD processID) {
    std::vector<DWORD> threadIDs;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return threadIDs;

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(snapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == processID) {
                threadIDs.push_back(te32.th32ThreadID);
            }
        } while (Thread32Next(snapshot, &te32));
    }
    CloseHandle(snapshot);
    return threadIDs;
}

LPVOID CustomAllocateMemory(HANDLE process, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) {
    LPVOID result = NULL;
    MEMORY_BASIC_INFORMATION mbi;

    for (SIZE_T addr = 0x1000; addr < 0x7FFFFFFF; addr += 0x1000) {
        if (!VirtualQueryEx(process, (LPCVOID)addr, &mbi, sizeof(mbi))) {
            continue;
        }

        if (mbi.State == MEM_FREE && mbi.RegionSize >= dwSize) {
            result = VirtualAllocEx(process, (LPVOID)addr, dwSize, flAllocationType, flProtect);
            if (result != NULL) {
                break;
            }
        }
    }

    return result;
}

BOOL CustomFreeMemory(HANDLE process, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) {
    BOOL result = VirtualFreeEx(process, lpAddress, dwSize, dwFreeType);
    if (!result) {
        std::wcerr << L"Failed to free memory in target process." << std::endl;
    }
    return result;
}

BOOL InjectDLL(DWORD processID, const std::wstring& dllPath) {
    HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, processID);
    if (processHandle == NULL) {
        std::wcerr << L"Failed to open target process." << std::endl;
        return FALSE;
    }

    size_t pathSize = (dllPath.size() + 1) * sizeof(wchar_t);
    LPVOID allocatedMemory = CustomAllocateMemory(processHandle, NULL, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (allocatedMemory == NULL) {
        std::wcerr << L"Failed to allocate memory in target process." << std::endl;
        CloseHandle(processHandle);
        return FALSE;
    }

    if (!WriteProcessMemory(processHandle, allocatedMemory, dllPath.c_str(), pathSize, NULL)) {
        std::wcerr << L"Failed to write process memory." << std::endl;
        CustomFreeMemory(processHandle, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return FALSE;
    }

    LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
    if (loadLibraryAddr == NULL) {
        std::wcerr << L"Failed to get address of LoadLibraryW." << std::endl;
        CustomFreeMemory(processHandle, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return FALSE;
    }

    std::vector<DWORD> threadIDs = GetThreadIDs(processID);
    if (threadIDs.empty()) {
        std::wcerr << L"Failed to find any threads in target process." << std::endl;
        CustomFreeMemory(processHandle, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return FALSE;
    }

    DWORD threadID = 0;
    for (DWORD tid : threadIDs) {
        HANDLE threadHandle = OpenThread(THREAD_SET_CONTEXT, FALSE, tid);
        if (threadHandle != NULL) {
            threadID = tid;
            CloseHandle(threadHandle);
            break;
        }
    }

    if (!threadID) {
        std::wcerr << L"Failed to set context for any thread in target process." << std::endl;
        CustomFreeMemory(processHandle, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return FALSE;
    }

    HANDLE remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, allocatedMemory, 0, NULL);
    if (remoteThread == NULL) {
        std::wcerr << L"Failed to create remote thread in target process." << std::endl;
        CustomFreeMemory(processHandle, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return FALSE;
    }

    WaitForSingleObject(remoteThread, INFINITE);
    CloseHandle(remoteThread);
    CustomFreeMemory(processHandle, allocatedMemory, 0, MEM_RELEASE);
    CloseHandle(processHandle);

    return TRUE;
}

BOOL OverrideFunction(DWORD processID, LPVOID targetAddress, LPVOID newFunction) {
    HANDLE process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, processID);
    if (process == NULL) {
        std::wcerr << L"Failed to open target process." << std::endl;
        return FALSE;
    }

    LPVOID functionAddress = VirtualAllocEx(process, NULL, sizeof(newFunction), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (functionAddress == NULL) {
        std::wcerr << L"Failed to allocate memory in target process." << std::endl;
        CloseHandle(process);
        return FALSE;
    }

    if (!WriteProcessMemory(process, functionAddress, newFunction, sizeof(newFunction), NULL)) {
        std::wcerr << L"Failed to write process memory." << std::endl;
        VirtualFreeEx(process, functionAddress, 0, MEM_RELEASE);
        CloseHandle(process);
        return FALSE;
    }

    if (!WriteProcessMemory(process, targetAddress, &functionAddress, sizeof(LPVOID), NULL)) {
        std::wcerr << L"Failed to override function." << std::endl;
        VirtualFreeEx(process, functionAddress, 0, MEM_RELEASE);
        CloseHandle(process);
        return FALSE;
    }

    FlushInstructionCache(process, functionAddress, sizeof(newFunction));
    CloseHandle(process);

    return TRUE;
}