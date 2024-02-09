#pragma once

#include <windows.h>
#include <vector>
#include <string>

DWORD FindProcessID(const std::wstring& processName);
LPVOID AllocateMemory(HANDLE process, size_t size);
void FreeMemory(HANDLE process, LPVOID ptr);
BOOL QueueAPCInject(DWORD processID, const std::wstring& dllPath);
std::vector<DWORD> GetThreadIDs(DWORD processID);
LPVOID CustomAllocateMemory(HANDLE process, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
BOOL CustomFreeMemory(HANDLE process, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
BOOL InjectDLL(DWORD processID, const std::wstring& dllPath);
BOOL OverrideFunction(DWORD processID, LPVOID targetAddress, LPVOID newFunction);
