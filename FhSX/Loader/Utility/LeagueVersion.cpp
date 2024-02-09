#include "LeagueVersion.h"
#include <iostream>
#include <vector>
#include <windows.h>

std::string filePath = "C:\\Riot Games\\League of Legends\\Game\\LeagueOfLegends.exe";

std::string GetFileVersion(const std::string& filePath) {
    DWORD dummy;
    DWORD versionSize = GetFileVersionInfoSize(filePath.c_str(), &dummy);
    if (versionSize == 0) {
        std::cerr << "Nie można uzyskać informacji o wersji pliku." << std::endl;
        return "";
    }

    std::vector<BYTE> versionData(versionSize);
    if (!GetFileVersionInfo(filePath.c_str(), 0, versionSize, versionData.data())) {
        std::cerr << "Nie można odczytać danych wersji pliku." << std::endl;
        return "";
    }

    VS_FIXEDFILEINFO* fileInfo;
    UINT fileInfoSize;
    if (VerQueryValue(versionData.data(), "\\", (LPVOID*)&fileInfo, &fileInfoSize)) {
        WORD majorVersion = HIWORD(fileInfo->dwFileVersionMS);
        WORD minorVersion = LOWORD(fileInfo->dwFileVersionMS);
        WORD buildNumber = HIWORD(fileInfo->dwFileVersionLS);
        WORD revisionNumber = LOWORD(fileInfo->dwFileVersionLS);

        return std::to_string(majorVersion) + "." +
            std::to_string(minorVersion) + "." +
            std::to_string(buildNumber) + "." +
            std::to_string(revisionNumber);
    }
    else {
        std::cerr << "Nie można odczytać danych wersji pliku." << std::endl;
        return "";
    }
}
