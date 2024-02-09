#include "Settings.h"
#include <fstream>
#include <filesystem>


std::wstring defaultLogsPath = L"C:\\Riot Games\\League of Legends\\Logs";
std::wstring defaultDllPath = L"C:\\Hanbot\\core.dll";

bool LoadUserSettings(std::wstring& logsPath, std::wstring& dllPath) {
    std::filesystem::path settingsPath = std::filesystem::current_path() / "Settings.txt";
    if (!std::filesystem::exists(settingsPath)) {
        logsPath = defaultLogsPath;
        dllPath = defaultDllPath;
        SaveUserSettings(logsPath, dllPath);
    }
    else {
        std::wifstream file(settingsPath);
        if (file.is_open()) {
            std::getline(file, logsPath);
            std::getline(file, dllPath);
            file.close();
        }
        else {
            return false;
        }
    }
    return true;
}

void SaveUserSettings(const std::wstring& logsPath, const std::wstring& dllPath) {
    std::wofstream file(std::filesystem::current_path() / "Settings.txt");
    if (file.is_open()) {
        file << logsPath << std::endl;
        file << dllPath << std::endl;
        file.close();
    }
}