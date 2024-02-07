#ifndef LEAGUEDRAGONS_H
#define LEAGUEDRAGONS_H

#include <string>

class LeagueDragons {
public:
    static std::string GetProductVersion(const std::string& filePath);
    static void DisplayProductVersion(const std::string& filePath);
    static void CheckGameVersion();

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    static std::string DownloadJson(const std::string& url);
    static std::string GetGameVersionFromJson(const std::string& jsonString);
};

#endif
