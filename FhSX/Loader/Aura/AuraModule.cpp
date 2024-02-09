#include "AuraModule.h"
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <string>
#include "LeagueFunctions.h"
#include "LeagueOverlay.h"
#include "Settings.h"
#include "Thread"


extern std::wstring GenerateRandomDllName(const std::wstring& directory);
extern std::wstring DecodeProcessName(const std::wstring& encodedName);
extern DWORD FindProcessId(const std::wstring& processName);
extern BOOL InjectDllIntoProcess(DWORD processID, const std::wstring& dllPath);
extern void ClearLogs(const std::wstring& logsPath);
extern BOOL OverrideFunction(DWORD processID, LPVOID targetAddress, LPVOID newFunction);
extern void DisableInternet();
extern void EnableInternet();
extern void bypass_crc(void* league_section_address, size_t league_section_size);

void RunInjectionProcess() {

    system("cls");

    wchar_t executablePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, executablePath, MAX_PATH) == 0) {
        std::wcerr << L"Error obtaining the executable path." << std::endl;
        return;
    }

    std::locale::global(std::locale(""));
    SetConsoleOutputCP(CP_UTF8);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD saved_attributes = consoleInfo.wAttributes;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    SetConsoleTextAttribute(hConsole, saved_attributes);

    std::wstring logsPath, dllPath;
    if (!LoadUserSettings(logsPath, dllPath)) {
        std::wcerr << L"Could not load user settings." << std::endl;
        return;
    }

    if (std::filesystem::exists(dllPath)) {
        std::wstring newDllPath = GenerateRandomDllName(L"C:\\Hanbot\\");
        try {
            std::filesystem::rename(dllPath, newDllPath);
            dllPath = newDllPath;
            SaveUserSettings(logsPath, dllPath);
            std::wcout << L"DLL renamed and settings updated: " << dllPath << std::endl;
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::wcerr << L"Error renaming DLL: " << e.what() << std::endl;
            return;
        }
    }
    else {
        std::wcerr << L"DLL path from settings does not exist. Checking for default DLL." << std::endl;
        std::wstring defaultDllPath = L"C:\\Hanbot\\core.dll";
        if (std::filesystem::exists(defaultDllPath)) {
            std::wstring newDllPath = GenerateRandomDllName(L"C:\\Hanbot\\");
            std::filesystem::rename(defaultDllPath, newDllPath);
            dllPath = newDllPath;
            SaveUserSettings(logsPath, dllPath);
            std::wcout << L"Default DLL renamed and settings updated: " << dllPath << std::endl;
        }
        else {
            std::wcerr << L"Default DLL also does not exist. Cannot proceed." << std::endl;
            return;
        }
    }

    ClearLogs(logsPath);


    std::wstring encodedProcessName = L"exe.sdnegeL fo eugaeL";
    std::wstring targetProcess = DecodeProcessName(encodedProcessName);

    DWORD processID = 0;
    static bool injected = false; 

    system("cls");

    while (true) {
        processID = FindProcessID(targetProcess); 
        if (processID != 0 && !injected) {
                std::wcout << targetProcess << L" Located. Initializing Hanbot" << std::endl;
                DisableInternet();
                if (InjectDLL(processID, dllPath)) { 
                    std::wcout << L"Hanbot initialized successfully." << std::endl;


                    EnableInternet();

                    injected = true; 

                    LPVOID fn_get_key_state = (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "GetAsyncKeyState");
                    LPVOID fn_issue_order = (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "IssueOrder");
                    LPVOID fn_cast_spell = (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "SendSpellCastPacket");
                    LPVOID fn_update_spell = (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "UpdateChargedSpell");

                    if (!fn_get_key_state || !fn_issue_order || !fn_cast_spell || !fn_update_spell) {
                        std::wcerr << L" " << std::endl;

                    }
                    else {

                        OverrideFunction(processID, fn_get_key_state, (LPVOID)GetAsyncKeyStateSingle);
                        OverrideFunction(processID, fn_issue_order, (LPVOID)IssueOrder);
                        OverrideFunction(processID, fn_cast_spell, (LPVOID)SendSpellCastPacket);
                        OverrideFunction(processID, fn_update_spell, (LPVOID)UpdateChargedSpell);

                        std::wcout << L"Bot functions overwritten successfully." << std::endl;
                    }

                }
                else {
                    std::wcerr << L"Initialization unsuccessful." << std::endl;
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

    return;

}

void StartInjectionProcess() {

        std::thread injectionThread(RunInjectionProcess);
        injectionThread.detach();

}