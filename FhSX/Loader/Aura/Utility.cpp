#include "Utility.h"
#include <random>
#include <algorithm> 
#include <iostream>
#include "Windows.h"
#include <filesystem>


std::wstring GenerateRandomDllName(const std::wstring& directory) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::wstring hexStr;
    for (int i = 0; i < 8; ++i) {
        hexStr += L"0123456789ABCDEF"[dis(gen)];
    }
    return directory + L"\\" + hexStr + L".dll";
}

std::wstring DecodeProcessName(const std::wstring& encodedName) {
    std::wstring decodedName = encodedName;
    std::reverse(decodedName.begin(), decodedName.end());
    return decodedName;
}

void ClearLogs(const std::wstring& logsPath) {
    std::wcout << L"Path received: " << logsPath << std::endl;

    if (!std::filesystem::exists(logsPath)) {
        std::wcout << L"Path not found: " << logsPath << std::endl;
        return;
    }

    try {
        for (const auto& entry : std::filesystem::directory_iterator(logsPath)) {
            try {
                std::filesystem::remove_all(entry.path());
            }
            catch (const std::filesystem::filesystem_error&) {

            }
        }
        std::wcout << L"Logs deleted successfully." << std::endl;
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::wcerr << L"Error to access the logs: " << e.what() << std::endl;
    }
}