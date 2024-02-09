#include "ServiceController.h"
#include <iostream>
#include "xorstr.h"

bool StopService() { 
    const wchar_t* serviceName = xorstr(L"vgc").crypt_get();
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        std::wcerr << xorstr(L"OpenSCManager failed, error code: ").crypt_get() << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        std::wcerr << xorstr(L"OpenService failed, error code: ").crypt_get() << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return false;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
        std::wcerr << xorstr(L"QueryServiceStatusEx failed, error code: ").crypt_get() << GetLastError() << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return false;
    }

    if (ssp.dwCurrentState != SERVICE_STOPPED && ssp.dwCurrentState != SERVICE_STOP_PENDING) {
        if (!ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp)) {
            std::wcerr << xorstr(L"ControlService failed, error code: ").crypt_get() << GetLastError() << std::endl;
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return false;
        }
    }

    std::wcout << xorstr(L"Service ").crypt_get() << serviceName << xorstr(L"' stopped successfully.").crypt_get() << std::endl;
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return true;
}
