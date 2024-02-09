#pragma once

#include <string>

std::wstring GenerateRandomDllName(const std::wstring& directory);
std::wstring DecodeProcessName(const std::wstring& encodedName);

void ClearLogs();
