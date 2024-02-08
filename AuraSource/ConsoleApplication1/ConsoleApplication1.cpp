#define NOMINMAX // x
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "wininet.lib")

#include <windows.h>
#include <winhttp.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <winternl.h>
#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <wincrypt.h>
#include <wininet.h>
#include <limits>
#include <cstdlib>  /
#include <ctime>    
#include <fstream>
#include <random>
#include <excpt.h>

const std::wstring CURRENT_VERSION = L"1.2.1"; // version



std::wstring DownloadVersionJson() {
    HINTERNET hSession = WinHttpOpen(L"Seu Programa/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET hConnect = WinHttpConnect(hSession, L"89.117.17.63", INTERNET_DEFAULT_HTTP_PORT, 0);
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/version.json", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

    if (!hRequest) {
        if (hConnect) WinHttpCloseHandle(hConnect);
        if (hSession) WinHttpCloseHandle(hSession);
        return L"";
    }

    WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    WinHttpReceiveResponse(hRequest, NULL);

    std::wstring response;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    do {
        dwSize = 0;
        WinHttpQueryDataAvailable(hRequest, &dwSize);
        pszOutBuffer = new char[dwSize + 1];
        if (!pszOutBuffer) break;

        ZeroMemory(pszOutBuffer, dwSize + 1);
        if (WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
            std::string str(pszOutBuffer, dwDownloaded);
            std::wstring wstr(str.begin(), str.end());
            response += wstr;
        }
        delete[] pszOutBuffer;
    } while (dwSize > 0);

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return response;
}


bool ExtractVersionAndURL(const std::wstring& json, std::wstring& version, std::wstring& url) {
    size_t versionPos = json.find(L"\"version\":");
    size_t urlPos = json.find(L"\"download_url\":");
    if (versionPos == std::wstring::npos || urlPos == std::wstring::npos) {
        return false;
    }

    size_t versionStart = json.find(L"\"", versionPos + 10) + 1;
    size_t versionEnd = json.find(L"\"", versionStart);
    size_t urlStart = json.find(L"\"", urlPos + 15) + 1;
    size_t urlEnd = json.find(L"\"", urlStart);

    if (versionStart == std::wstring::npos || versionEnd == std::wstring::npos ||
        urlStart == std::wstring::npos || urlEnd == std::wstring::npos) {
        return false;
    }

    version = json.substr(versionStart, versionEnd - versionStart);
    url = json.substr(urlStart, urlEnd - urlStart);

    return true;
}

bool DownloadFile(const std::wstring& url, const std::wstring& destPath) {
    HINTERNET hSession = WinHttpOpen(L"Seu Programa/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    URL_COMPONENTS urlComp;
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);

    
    wchar_t hostName[256];
    wchar_t urlPath[256];
    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = 256;
    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = 256;

    
    if (!WinHttpCrackUrl(url.c_str(), url.length(), 0, &urlComp)) {
        return false;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, urlComp.lpszHostName, urlComp.nPort, 0);
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        return false;
    }

    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        return false;
    }

    std::ofstream outFile(destPath, std::ios::binary);
    if (!outFile.is_open()) {
        return false;
    }

    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    bool bResult = true;
    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            bResult = false;
            break;
        }

        pszOutBuffer = new char[dwSize + 1];
        if (!pszOutBuffer) {
            bResult = false;
            break;
        }

        ZeroMemory(pszOutBuffer, dwSize + 1);
        if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
            bResult = false;
        }
        else {
            outFile.write(pszOutBuffer, dwDownloaded);
        }

        delete[] pszOutBuffer;

        if (!bResult) {
            break;
        }
    } while (dwSize > 0);

    outFile.close();

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return bResult;
}



bool CheckAndUpdateVersion() {
    std::wstring json = DownloadVersionJson();
    std::wstring latestVersion, downloadUrl;
    if (!ExtractVersionAndURL(json, latestVersion, downloadUrl)) {
        return false;
    }

    if (latestVersion != CURRENT_VERSION) {
        std::wcout << L"Iniciando a atualização para a versão " << latestVersion << L"..." << std::endl;

        wchar_t exePath[MAX_PATH];
        if (GetModuleFileNameW(NULL, exePath, MAX_PATH) == 0) {
            std::wcout << L"Falha ao obter o caminho do executável." << std::endl;
            return false; // Falha ao obter o caminho do executável
        }

        std::wstring exeFolderPath = exePath;
        size_t lastBackslashIndex = exeFolderPath.find_last_of(L"\\");
        if (lastBackslashIndex != std::wstring::npos) {
            exeFolderPath = exeFolderPath.substr(0, lastBackslashIndex);
        }

        std::wstring tempDownloadPath = exeFolderPath + L"\\temp_new_version.exe";

        if (!DownloadFile(downloadUrl, tempDownloadPath)) {
            std::wcout << L"Falha ao baixar a nova versão." << std::endl;
            return false; // fail download
        }

        std::wstring oldExecutablePath = exePath;
        std::wstring backupExecutablePath = oldExecutablePath + L".bak";

        DeleteFile(backupExecutablePath.c_str());

        if (!MoveFileEx(oldExecutablePath.c_str(), backupExecutablePath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            std::wcout << L"Falha ao renomear o executável antigo." << std::endl;
            return false; // ok
        }

        if (!MoveFileEx(tempDownloadPath.c_str(), oldExecutablePath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            std::wcout << L"Falha ao substituir o executável." << std::endl;
            return false; // mov fail
        }

        std::wcout << L"Update Completed. Open the new loader." << std::endl;
        system("start cmd /k echo Update Completed. Open the new loader");
        return true;
    }

    std::wcout << L"Bypass is updated." << std::endl;
    return false;
}







std::wstring PolimorphicEncryptDecrypt(const std::wstring& str) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::wstring key;
    for (int i = 0; i < 8; ++i) {
        key += L"0123456789ABCDEF"[dis(gen)];
    }

    std::wstring result = str;
    for (size_t i = 0; i < str.size(); i++) {
        result[i] = str[i] ^ key[i % key.size()];
    }
    return result;
}

bool StopService(const wchar_t* serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, RESOURCEMANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        std::wcerr << L"Failed" << GetLastError() << std::endl;
        return false;
    }
    SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        std::wcerr << L"Falhou dnv" << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return false;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
        std::wcerr << L"QueryServiceStatusEx falha" << GetLastError() << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return false;
    }

    if (ssp.dwCurrentState != SERVICE_STOPPED && ssp.dwCurrentState != SERVICE_STOP_PENDING) {
        if (!ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp)) {
            std::wcerr << L"Falha" << GetLastError() << std::endl;
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return false;
        }
    }
    std::wcout << L"Serviço " << serviceName << L" parado." << std::endl;
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return true;
}

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

DWORD FindProcessId(const std::wstring& processName) {
    PROCESSENTRY32W processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32FirstW(processesSnapshot, &processInfo)) {
        do {
            if (std::wstring(processInfo.szExeFile) == processName) {
                CloseHandle(processesSnapshot);
                return processInfo.th32ProcessID;
            }
        } while (Process32NextW(processesSnapshot, &processInfo));
    }

    CloseHandle(processesSnapshot);
    return 0;
}

std::string wstring_to_string(const std::wstring& wstr) {
    std::vector<char> buffer(wstr.size() * 2);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &buffer[0], buffer.size(), NULL, NULL);
    return std::string(buffer.begin(), buffer.end());
}

std::wstring OfuscarString(const std::wstring& strOriginal) {
    std::wstring strOfuscada;
    for (wchar_t c : strOriginal) {
        strOfuscada.push_back(c + 1);
    }
    return strOfuscada;
}




// Function declarations
LPVOID MyAlloc(HANDLE cxvvxa, size_t size);
void MyFree(HANDLE cxvvxa, LPVOID ptr);
BOOL QueueAPCInject(DWORD processID, const std::wstring& dllPath);


LPVOID MyAlloc(HANDLE cxvvxa, size_t size) {
    return VirtualAllocEx(cxvvxa, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void MyFree(HANDLE cxvvxa, LPVOID ptr) {
    VirtualFreeEx(cxvvxa, ptr, 0, MEM_RELEASE);
}
// Função auxiliar para obter a lista de threads de um processo
typedef NTSTATUS(NTAPI* pfnNtQueryInformationThread)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG);

std::vector<DWORD> vxvxbxa(DWORD processID) {
    std::vector<DWORD> threadIDs;
    HANDLE bxabasSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (bxabasSnap == INVALID_HANDLE_VALUE) return threadIDs;

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(bxabasSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == processID) {
                threadIDs.push_back(te32.th32ThreadID);
            }
        } while (Thread32Next(bxabasSnap, &te32));
    }
    CloseHandle(bxabasSnap);
    return threadIDs;
}

// mem aloc
LPVOID casdasd(HANDLE cxvvxa, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) {
    LPVOID result = NULL;
    MEMORY_BASIC_INFORMATION mbi;

    
    for (SIZE_T addr = 0x1000; addr < 0x7FFFFFFF; addr += 0x1000) {
        if (!VirtualQueryEx(cxvvxa, (LPCVOID)addr, &mbi, sizeof(mbi))) {
            continue;
        }

        if (mbi.State == MEM_FREE && mbi.RegionSize >= dwSize) {
            result = VirtualAllocEx(cxvvxa, (LPVOID)addr, dwSize, flAllocationType, flProtect);
            if (result != NULL) {
                break;
            }
        }
    }

    return result;
}

// mem release
BOOL asdacavba(HANDLE cxvvxa, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) {
    BOOL result = VirtualFreeEx(cxvvxa, lpAddress, dwSize, dwFreeType);
    if (!result) {
        std::wcerr << L"Failed to free memory in target process." << std::endl;
    }
    return result;
}


BOOL cvabdafad(DWORD processID, const std::wstring& dllPath) {
    HANDLE cxvvxa = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, processID);
    if (cxvvxa == NULL) {
        std::wcerr << L"Failed to open target process." << std::endl;
        return FALSE;
    }

   
    size_t pathSize = (dllPath.size() + 1) * sizeof(wchar_t);
    LPVOID vxxasda = casdasd(cxvvxa, NULL, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (vxxasda == NULL) {
        std::wcerr << L"Failed to allocate memory in target process." << std::endl;
        CloseHandle(cxvvxa);
        return FALSE;
    }

 
    if (!WriteProcessMemory(cxvvxa, vxxasda, dllPath.c_str(), pathSize, NULL)) {
        std::wcerr << L"Failed to write process memory." << std::endl;
        asdacavba(cxvvxa, vxxasda, 0, MEM_RELEASE);
        CloseHandle(cxvvxa);
        return FALSE;
    }

    
    LPVOID bbxbzxz = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
    if (bbxbzxz == NULL) {
        std::wcerr << L"Failed to get address of LoadLibraryW." << std::endl;
        asdacavba(cxvvxa, vxxasda, 0, MEM_RELEASE);
        CloseHandle(cxvvxa);
        return FALSE;
    }

    
    std::vector<DWORD> threadIDs = vxvxbxa(processID);
    if (threadIDs.empty()) {
        std::wcerr << L"x." << std::endl;
        asdacavba(cxvvxa, vxxasda, 0, MEM_RELEASE);
        CloseHandle(cxvvxa);
        return FALSE;
    }

    
    DWORD threadID = 0;
    for (DWORD tid : threadIDs) {
        HANDLE bxabas = OpenThread(THREAD_SET_CONTEXT, FALSE, tid);
        if (bxabas != NULL) {
            threadID = tid;
            CloseHandle(bxabas);
            break;
        }
    }

    if (!threadID) {
        std::wcerr << L"x." << std::endl;
        asdacavba(cxvvxa, vxxasda, 0, MEM_RELEASE);
        CloseHandle(cxvvxa);
        return FALSE;
    }


    HANDLE bxabas = CreateRemoteThread(cxvvxa, NULL, 0, (LPTHREAD_START_ROUTINE)bbxbzxz, vxxasda, 0, NULL);
    if (bxabas == NULL) {
        std::wcerr << L"x." << std::endl;
        asdacavba(cxvvxa, vxxasda, 0, MEM_RELEASE);
        CloseHandle(cxvvxa);
        return FALSE;
    }

    
    WaitForSingleObject(bxabas, INFINITE);
    CloseHandle(bxabas);

    
    asdacavba(cxvvxa, vxxasda, 0, MEM_RELEASE);

    
    CloseHandle(cxvvxa);

    return TRUE;
}


BOOL funcover(DWORD processID, LPVOID targetAddress, LPVOID newFunction) {
    HANDLE cxvvxa = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, processID);
    if (cxvvxa == NULL) {
        std::wcerr << L"nothing." << std::endl;
        return FALSE;
    }

                      
    LPVOID vxxasda = VirtualAllocEx(cxvvxa, NULL, sizeof(newFunction), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (vxxasda == NULL) {
        std::wcerr << L"alloc fail." << std::endl;
        CloseHandle(cxvvxa);
        return FALSE;
    }

    
    if (!WriteProcessMemory(cxvvxa, vxxasda, newFunction, sizeof(newFunction), NULL)) {
        std::wcerr << L"write fail." << std::endl;
        VirtualFreeEx(cxvvxa, vxxasda, 0, MEM_RELEASE);
        CloseHandle(cxvvxa);
        return FALSE;
    }

   
    if (!WriteProcessMemory(cxvvxa, targetAddress, &vxxasda, sizeof(LPVOID), NULL)) {
        std::wcerr << L"write fail too." << std::endl;
        VirtualFreeEx(cxvvxa, vxxasda, 0, MEM_RELEASE);
        CloseHandle(cxvvxa);
        return FALSE;
    }

   
    FlushInstructionCache(cxvvxa, vxxasda, sizeof(newFunction));


    CloseHandle(cxvvxa);

    return TRUE;
}




bool CheckUserKey(const std::wstring& username, const std::wstring& userkey) {
    HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET hConnect = WinHttpConnect(hSession, L"89.117.17.63", INTERNET_DEFAULT_HTTP_PORT, 0);
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/credenciais.json", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

    if (hRequest) {
        WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
        WinHttpReceiveResponse(hRequest, NULL);

        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        LPSTR pszOutBuffer;
        std::wstring response;
        BOOL bResult = TRUE;

        do {
            dwSize = 0;
            WinHttpQueryDataAvailable(hRequest, &dwSize);
            pszOutBuffer = new char[dwSize + 1];
            if (pszOutBuffer) {
                ZeroMemory(pszOutBuffer, dwSize + 1);
                if (WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                    std::string str(pszOutBuffer, dwDownloaded);
                    std::wstring wstr(str.begin(), str.end());
                    response += wstr;
                }
                delete[] pszOutBuffer;
            }
        } while (dwSize > 0);

        std::wstring userToken = L"\"username\": \"" + username + L"\"";
        std::wstring keyToken = L"\"userkey\": \"" + userkey + L"\"";
        if (response.find(userToken) != std::wstring::npos && response.find(keyToken) != std::wstring::npos) {
            return true;
        }
    }

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return false;
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
                // Error handling
            }
        }
        std::wcout << L"Logs deleted successfully." << std::endl;
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::wcerr << L"Error to access the logs: " << e.what() << std::endl;
    }
}

bool LoadUserSettings(std::wstring& username, std::wstring& userkey, std::wstring& logsPath, std::wstring& dllPath) {
    std::wifstream file("user_settings.txt");
    if (file.is_open()) {
        std::getline(file, username);
        std::getline(file, userkey);
        std::getline(file, logsPath);
        std::getline(file, dllPath);
        file.close();
        return true;
    }
    return false;
}

void SaveUserSettings(const std::wstring& username, const std::wstring& userkey, const std::wstring& logsPath, const std::wstring& dllPath) {
    std::wofstream file("user_settings.txt");
    if (file.is_open()) {
        file << username << std::endl;
        file << userkey << std::endl;
        file << logsPath << std::endl;
        file << dllPath << std::endl;
        file.close();
    }
}

std::wstring GenerateRandomExeName() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::wstring hexStr;
    for (int i = 0; i < 8; ++i) {
        hexStr += L"0123456789ABCDEF"[dis(gen)];
    }
    return hexStr + L".exe";
}

void CreateRenamingBatch(const std::wstring& executablePath, const std::wstring& newExeName) {
    std::wstring batFilename = L"rename_script.bat";
    std::wofstream batchFile(batFilename);
    batchFile << L"@echo off\n";
    batchFile << L"rename \"" << executablePath << L"\" \"" << newExeName << L"\"\n";
    batchFile << L"del \"" << batFilename << L"\"\n";  
    batchFile << L"exit\n";  
    batchFile.close();

    
    std::wstring command = L"cmd.exe /C start /b " + batFilename;
    _wsystem(command.c_str());
}

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


struct LeagueHash {
    uint64_t xored_hash;
    uint64_t xor_key;
};


uint64_t fnv1a64(const void* data, size_t size) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < size; i++) {
        hash ^= ((uint8_t*)data)[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}


void bypass_crc(void* league_section_address, size_t league_section_size) {
    
    LeagueHash* league_hash_array = (LeagueHash*)GetProcAddress(GetModuleHandle(L"stub.dll"), "LeagueHashArray");

    
    for (size_t i = 0; i < league_section_size; i += 0x1000) {
        uint64_t hash = fnv1a64((void*)((uint8_t*)league_section_address + i), 0x1000);

        
        hash ^= league_hash_array[i / 0x1000].xor_key;

        
        if (hash != league_hash_array[i / 0x1000].xored_hash) {
            
            hash = fnv1a64((void*)((uint8_t*)league_section_address + i), 0x1000);
            hash ^= league_hash_array[i / 0x1000].xor_key;

           
            league_hash_array[i / 0x1000].xored_hash = hash;
        }
    }
}


static LPVOID GetAsyncKeyStateSingle(int vKey);
static LPVOID IssueOrder(int unitID, int orderID);
static LPVOID SendSpellCastPacket(int unitID, int spellID, int targetIndex);
static LPVOID UpdateChargedSpell(int unitID, int spellID, int targetIndex);
int main() {
    wchar_t executablePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, executablePath, MAX_PATH) == 0) {
        std::wcerr << L"Error to obtain the executable." << std::endl;
        return 1;
    }

    std::locale::global(std::locale(""));
    SetConsoleOutputCP(CP_UTF8);

    
    if (CheckAndUpdateVersion()) {
         
        std::wcout << L"Atualização realizada. Por favor, reinicie o aplicativo." << std::endl;
        return 0;
    }

    
    std::wcout << L"    A     U   U  RRRR      A   " << std::endl;
    std::wcout << L"   A A    U   U  R   R    A A  " << std::endl;
    std::wcout << L"  AAAAA   U   U  RRRR    AAAAA " << std::endl;
    std::wcout << L" A     A  U   U  R  R   A     A" << std::endl;
    std::wcout << L"A       A UUUUU  R   R A       A" << std::endl << std::endl;
     
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

     
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD saved_attributes = consoleInfo.wAttributes;

 
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

    
    std::wcout << L"If you bought this, you've been scammed." << std::endl;
    std::wcout << L"Discord: https://discord.gg/ptvD9u3JxH" << std::endl;

 
    SetConsoleTextAttribute(hConsole, saved_attributes);

    std::wstring username, userkey, logsPath, dllPath;

    if (!LoadUserSettings(username, userkey, logsPath, dllPath) || !std::filesystem::exists(dllPath)) {
        std::wcout << L"Username: ";
        std::getline(std::wcin, username);
        std::wcout << L"Password: ";
        std::getline(std::wcin, userkey);
        std::wcout << L"Type the path for the Logs of League Of Legends. Ex: C:\\Riot Games\\League of Legends\\Logs: ";
        std::getline(std::wcin, logsPath);

        std::wstring defaultDllPath = L"C:\\hanbot\\league of legends\\core.dll";

        dllPath = GenerateRandomDllName(L"C:\\hanbot\\league of legends");
        std::filesystem::rename(defaultDllPath, dllPath);

        SaveUserSettings(username, userkey, logsPath, dllPath);
    }
    else {
        std::wcout << L"Information loaded from a config file." << std::endl;
    }

    if (!CheckUserKey(username, userkey)) {
        std::wcerr << L"Password incorrect." << std::endl;
        Sleep(3000);  
        return 1;
    }

    std::wcout << L"Password valid, running..." << std::endl;

    ClearLogs(logsPath);

    if (!StopService(L"vgc")) {
        std::wcerr << L"Error to stop vanguard (don't have or is not running)" << std::endl;
    }

     
    std::wstring encodedProcessName = L"exe.sdnegeL fo eugaeL"; 

     
    std::wstring targetProcess = DecodeProcessName(encodedProcessName);

     
    std::wcout << L"Monitoring for " << targetProcess << L"..." << std::endl;

     
    DWORD processID = 0;
    bool injected = false;

  
    std::wcout << L"Press 'Z' to inject when in loading..." << std::endl;

    
    while (true) {
         
        processID = FindProcessId(targetProcess);

         
        if (processID != 0 && !injected) {
             
            if (GetAsyncKeyState('Z') & 0x8000) {  
                 
                std::wcout << targetProcess << L" found... Injecting!" << std::endl;

                
                DisableInternet();

                
                if (cvabdafad(processID, dllPath)) {
                    
                    std::wcout << L"DLL successfully injected." << std::endl;

                     
                    EnableInternet();

                    
                    injected = true;

                    LPVOID fn_get_key_state = (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dlll"), "GetAsyncKeyState");
                    LPVOID fn_issue_order = (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "IssueOrder");
                    LPVOID fn_cast_spell = (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "SendSpellCastPacket");
                    LPVOID fn_update_spell = (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "UpdateChargedSpell");

                   
                    funcover(processID, fn_get_key_state, GetAsyncKeyStateSingle);
                    funcover(processID, fn_issue_order, IssueOrder);
                    funcover(processID, fn_cast_spell, SendSpellCastPacket);
                    funcover(processID, fn_update_spell, UpdateChargedSpell);

                    std::wcout << L"Bot functions overwritten successfully." << std::endl;
                }
                else {
                    
                    std::wcerr << L"Error injecting DLL." << std::endl;
                }
            }
        }
        else if (processID == 0) {
            
            injected = false;
        }

       
        Sleep(100);
    }
    
    PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
    PIMAGE_NT_HEADERS nt_headers = (PIMAGE_NT_HEADERS)((uint8_t*)dos_header + dos_header->e_lfanew);
    void* league_section_address = (void*)((uint8_t*)dos_header + nt_headers->OptionalHeader.ImageBase + nt_headers->OptionalHeader.SectionAlignment);

    
    size_t league_section_size = nt_headers->OptionalHeader.SizeOfImage;

    
    bypass_crc(league_section_address, league_section_size);

    return 0;
}

    
static LPVOID GetAsyncKeyStateSingle(int vKey) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "GetAsyncKeyState");
}

static LPVOID GetAsyncKeyState() {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "GetAsyncKeyState");
}

static LPVOID IssueOrder(int unitID, int orderID) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "IssueOrder");
}

static LPVOID IssueOrderWithTarget(int unitID, int orderID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "IssueOrder");
}

static LPVOID SendSpellCastPacket(int unitID, int spellID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "SendSpellCastPacket");
}

static LPVOID UpdateChargedSpell(int unitID, int spellID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "UpdateChargedSpell");
}