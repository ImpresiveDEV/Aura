#include "LeagueDragons.h"
#include <windows.h>
#include <winver.h>
#include <iostream>
#include <filesystem> 
#include <curl/curl.h>
#include "json.hpp"
#include "Config.h"
#include "xorstr.h"

namespace fs = std::filesystem;

static size_t WriteData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

std::string LeagueDragons::GetProductVersion(const std::string& filePath) {
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeA(filePath.c_str(), &handle);
    std::string productVersion = xorstr("Unknown League of Legends version").crypt_get();

    if (size) {
        BYTE* buffer = new BYTE[size];
        if (GetFileVersionInfoA(filePath.c_str(), handle, size, buffer)) {
            UINT size = 0;
            LPVOID lpBuffer = nullptr;
            if (VerQueryValueA(buffer, xorstr("\\StringFileInfo\\040904b0\\ProductVersion").crypt_get(), &lpBuffer, &size)) {
                productVersion = std::string((char*)lpBuffer);
            }
        }
        delete[] buffer;
    }

    return productVersion;
}

void LeagueDragons::DownloadFile(const std::string& url, const std::string& filePath) {
    CURL* curl;
    FILE* file = nullptr;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        errno_t err = fopen_s(&file, filePath.c_str(), "wb");
        if (err == 0) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); 
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); 

            res = curl_easy_perform(curl);
            fclose(file);

            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            std::cout << xorstr("HTTP Response code: ").crypt_get() << response_code << std::endl;

            if (res != CURLE_OK)
                std::cerr << xorstr("curl_easy_perform() failed: ").crypt_get() << curl_easy_strerror(res) << std::endl;
        }
        else {
            std::cerr << xorstr("Failed to open file: ").crypt_get() << filePath << std::endl;
        }
        curl_easy_cleanup(curl);
    }
}



void LeagueDragons::EnsureDirectoryExists(const std::string& path) {
    fs::path dir(path);
    if (!fs::exists(dir))
        fs::create_directories(dir);
}

void LeagueDragons::DownloadAndSaveFiles(const std::string& jsonString) {
    auto json = nlohmann::json::parse(jsonString);
    std::string basePath = xorstr("C:\\Hanbot").crypt_get();
    EnsureDirectoryExists(basePath);

    for (auto& file : json[xorstr("files").crypt_get()]) {
        std::string fileName = file[xorstr("Name").crypt_get()];

        std::string downloadLink = file.value(xorstr("downloadLink").crypt_get(), xorstr("").crypt_get());
        if (downloadLink.empty()) 
            downloadLink = file.value(xorstr("DownloadLink").crypt_get(), xorstr("").crypt_get());

        if (!downloadLink.empty()) {
            std::string filePath = basePath + xorstr("\\").crypt_get() + fileName;
            DownloadFile(downloadLink, filePath);
            std::cout << xorstr("Downloaded successfully: ").crypt_get() << fileName << std::endl;
        }
        else {
            std::cout << xorstr("Failed to find download link for: ").crypt_get() << fileName << std::endl;
        }
    }
}

void LeagueDragons::DisplayProductVersion(const std::string& filePath) {
    std::string version = GetProductVersion(filePath);
    std::cout << version << std::endl;
}

size_t LeagueDragons::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string LeagueDragons::DownloadJson(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << xorstr("curl_easy_perform() failed: ").crypt_get() << curl_easy_strerror(res) << std::endl;
            return "";
        }
    }
    return readBuffer;
}

std::string LeagueDragons::GetGameVersionFromJson(const std::string& jsonString) {
    auto json = nlohmann::json::parse(jsonString);
    std::string gameVersion = json[xorstr("GameVersion").crypt_get()];
    return gameVersion;
}

void LeagueDragons::CheckGameVersion() {

    std::string jsonContent = DownloadJson(jsonUrl);
    std::string gameVersionFromJson = GetGameVersionFromJson(jsonContent);
    std::string gameVersionFromFile = GetProductVersion(filePath);

    if (gameVersionFromJson == gameVersionFromFile) {
        std::cout << xorstr("Hanbot Core is up to date! The loaded version is: ").crypt_get() << gameVersionFromFile << std::endl;

        DownloadAndSaveFiles(jsonContent); 
    }
    else {
        std::cout << xorstr("Hanbot Core is Outdated. Wait for update Nexus Core summoner. The client now will close for 15 sec. Hanbot core version is: ").crypt_get() << gameVersionFromJson << xorstr(", League of Legends version is: ").crypt_get() << gameVersionFromFile << std::endl;
        Sleep(15000);
        exit(0);
    }
}