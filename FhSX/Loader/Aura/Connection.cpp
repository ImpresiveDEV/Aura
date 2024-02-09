#include "Connection.h"

void DisableInternet() {
    HINTERNET hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet != NULL) {
        InternetSetOption(hInternet, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
        InternetCloseHandle(hInternet);
    }
}

void EnableInternet() {
    HINTERNET hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet != NULL) {
        InternetSetOption(hInternet, INTERNET_OPTION_REFRESH, NULL, 0);
        InternetCloseHandle(hInternet);
    }
}
