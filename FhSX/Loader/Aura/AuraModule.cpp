#include "AuraModule.h"
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <string>
#include "LeagueFunctions.h"
#include "LeagueOverlay.h"
#include "Settings.h"
#include "xorstr.h"

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

    wchar_t executablePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, executablePath, MAX_PATH) == 0) {
        std::wcerr << xorstr(L"Error obtaining the executable path.").crypt_get() << std::endl;
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
        std::wcerr << xorstr(L"Could not load user settings.").crypt_get() << std::endl;
        return;
    }

    if (std::filesystem::exists(dllPath)) {
        std::wstring newDllPath = GenerateRandomDllName(xorstr(L"C:\\Hanbot\\").crypt_get());
        try {
            std::filesystem::rename(dllPath, newDllPath);
            dllPath = newDllPath;
            SaveUserSettings(logsPath, dllPath);
            std::wcout << xorstr(L"DLL renamed and settings updated: ").crypt_get() << dllPath << std::endl;
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::wcerr << xorstr(L"Error renaming DLL: ").crypt_get() << e.what() << std::endl;
            return;
        }
    }
    else {
        std::wcerr << xorstr(L"DLL path from settings does not exist. Checking for default DLL.").crypt_get() << std::endl;
        std::wstring defaultDllPath = xorstr(L"C:\\Hanbot\\core.dll").crypt_get();
        if (std::filesystem::exists(defaultDllPath)) {
            std::wstring newDllPath = GenerateRandomDllName(xorstr(L"C:\\Hanbot\\").crypt_get());
            std::filesystem::rename(defaultDllPath, newDllPath);
            dllPath = newDllPath;
            SaveUserSettings(logsPath, dllPath);
            std::wcout << xorstr(L"Default DLL renamed and settings updated: ").crypt_get() << dllPath << std::endl;
        }
        else {
            std::wcerr << xorstr(L"Default DLL also does not exist. Cannot proceed.").crypt_get() << std::endl;
            return;
        }
    }

    ClearLogs(logsPath);

    std::wstring encodedProcessName = xorstr(L"exe.sdnegeL fo eugaeL").crypt_get();
    std::wstring targetProcess = DecodeProcessName(encodedProcessName);
    std::wcout << xorstr(L"Monitoring for ").crypt_get() << targetProcess << xorstr(L"...").crypt_get() << std::endl;

    DWORD processID = 0;
    static bool injected = false; 

    std::wcout << xorstr(L"Press 'Z' to inject when in loading...").crypt_get() << std::endl;

    while (true) {
        processID = FindProcessID(targetProcess); 
        if (processID != 0 && !injected) {
            if (GetAsyncKeyState('Z') & 0x8000) {
                std::wcout << targetProcess << xorstr(L" found... Injecting!").crypt_get() << std::endl;
                DisableInternet();
                if (InjectDLL(processID, dllPath)) { 
                    std::wcout << xorstr(L"DLL successfully injected.").crypt_get() << std::endl;


                    EnableInternet();

                    injected = true; 

                    LPVOID fn_get_key_state = (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("GetAsyncKeyState").crypt_get());
                    LPVOID fn_issue_order = (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("IssueOrder").crypt_get());
                    LPVOID fn_cast_spell = (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("SendSpellCastPacket").crypt_get());
                    LPVOID fn_update_spell = (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("UpdateChargedSpell").crypt_get());

                    if (!fn_get_key_state || !fn_issue_order || !fn_cast_spell || !fn_update_spell) {
                        std::wcerr << xorstr(L" ").crypt_get() << std::endl;

                    }
                    else {

                        OverrideFunction(processID, fn_get_key_state, (LPVOID)GetAsyncKeyStateSingle);
                        OverrideFunction(processID, fn_issue_order, (LPVOID)IssueOrder);
                        OverrideFunction(processID, fn_cast_spell, (LPVOID)SendSpellCastPacket);
                        OverrideFunction(processID, fn_update_spell, (LPVOID)UpdateChargedSpell);

                        std::wcout << xorstr(L"Bot functions overwritten successfully.").crypt_get() << std::endl;
                    }

                }
                else {
                    std::wcerr << xorstr(L"Error injecting DLL.").crypt_get() << std::endl;
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

}