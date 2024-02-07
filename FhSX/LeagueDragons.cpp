#include "LeagueDragons.h"
#include <windows.h>
#include <winver.h>
#include <iostream>
#include <curl/curl.h>
#include "json.hpp"
#include "Config.h"

std::string LeagueDragons::GetProductVersion(const std::string& filePath) {
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSizeA(filePath.c_str(), &handle);
    std::string productVersion = "Nieznana wersja";

    if (size) {
        BYTE* buffer = new BYTE[size];
        if (GetFileVersionInfoA(filePath.c_str(), handle, size, buffer)) {
            UINT size = 0;
            LPVOID lpBuffer = nullptr;
            if (VerQueryValueA(buffer, "\\StringFileInfo\\040904b0\\ProductVersion", &lpBuffer, &size)) {
                productVersion = std::string((char*)lpBuffer);
            }
        }
        delete[] buffer;
    }

    return productVersion;
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
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }
    }
    return readBuffer;
}

std::string LeagueDragons::GetGameVersionFromJson(const std::string& jsonString) {
    auto json = nlohmann::json::parse(jsonString);
    std::string gameVersion = json["GameVersion"];
    return gameVersion;
}

void LeagueDragons::CheckGameVersion() {
    std::string jsonContent = DownloadJson(jsonUrl); 
    std::string gameVersionFromJson = GetGameVersionFromJson(jsonContent);

    std::string gameVersionFromFile = GetProductVersion(filePath);

    if (gameVersionFromJson == gameVersionFromFile) {
        std::cout << "Hanbot Core is up to date! The loaded version is: " << gameVersionFromFile << std::endl;
    }
    else {
        std::cout << "Hanbot Core is Outdated. Wait for update Nexus Core summoner. The client now will close for 15 sec. Hanbot core version is : " << gameVersionFromJson << ", League of Legends version is: : " << gameVersionFromFile << std::endl;
        Sleep(15000);
        exit(0);
    }
}
