#include "VersionCheck.h"
#include <windows.h>
#include <vector>
#include <iostream>

typedef DWORD(WINAPI* LPFN_GETFILEVERSIONINFOSIZEA)(LPCSTR lpstrFilename, LPDWORD lpdwHandle);

std::string GetFileVersion(const std::string& filePath) {
    DWORD dummy;
    DWORD versionSize = GetFileVersionInfoSizeA(filePath.c_str(), &dummy);
    if (versionSize == 0) {
        std::cerr << "Nie można uzyskać informacji o wersji pliku." << std::endl;
        return "";
    }

    std::vector<BYTE> versionData(versionSize);
    if (!GetFileVersionInfoA(filePath.c_str(), 0, versionSize, versionData.data())) {
        std::cerr << "Nie można odczytać danych wersji pliku." << std::endl;
        return "";
    }

    VS_FIXEDFILEINFO* fileInfo;
    UINT fileInfoSize;
    if (VerQueryValueA(versionData.data(), "\\", (LPVOID*)&fileInfo, &fileInfoSize)) {
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
