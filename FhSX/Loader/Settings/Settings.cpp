#include "Settings.h"
#include <fstream>
#include <filesystem>
#include "xorstr.h"

bool LoadUserSettings(std::wstring& logsPath, std::wstring& dllPath) {
    std::wifstream file(std::filesystem::current_path() / xorstr("Settings.txt").crypt_get());
    if (file.is_open()) {
        std::getline(file, logsPath);
        std::getline(file, dllPath);
        file.close();
        return true;
    }
    return false;
}

void SaveUserSettings(const std::wstring& logsPath, const std::wstring& dllPath) {
    std::wofstream file(std::filesystem::current_path() / xorstr("Settings.txt").crypt_get());
    if (file.is_open()) {
        file << logsPath << std::endl;
        file << dllPath << std::endl;
        file.close();
    }
}