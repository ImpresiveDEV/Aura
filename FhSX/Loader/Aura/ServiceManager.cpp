#include "ServiceManager.h"
#include <iostream>

bool ServiceManager::StopService(const std::wstring& serviceName) {
    SC_HANDLE schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == nullptr) {
        std::wcerr << L"OpenSCManager failed: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE schService = OpenService(schSCManager, serviceName.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (schService == nullptr) {
        std::wcerr << L"OpenService failed: " << GetLastError() << std::endl;
        CloseServiceHandle(schSCManager);
        return false;
    }

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    if (!QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
        std::wcerr << L"QueryServiceStatusEx failed: " << GetLastError() << std::endl;
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return false;
    }

    if (ssp.dwCurrentState != SERVICE_STOPPED && ssp.dwCurrentState != SERVICE_STOP_PENDING) {
        if (!ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp)) {
            std::wcerr << L"ControlService failed: " << GetLastError() << std::endl;
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return false;
        }
    }

    std::wcout << L"Service " << serviceName << L" stopped successfully." << std::endl;

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return true;
}
