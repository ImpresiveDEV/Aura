#pragma once

#include <string>

bool LoadUserSettings(std::wstring& logsPath, std::wstring& dllPath);
void SaveUserSettings(const std::wstring& logsPath, const std::wstring& dllPath);
