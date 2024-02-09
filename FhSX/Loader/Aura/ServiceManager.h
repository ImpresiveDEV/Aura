#pragma once

#include <windows.h>
#include <string>

class ServiceManager {
public:

    static bool StopService(const std::wstring& serviceName);

};
