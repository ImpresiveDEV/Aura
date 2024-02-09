// DxInit.h
#pragma once
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

class DxInit
{

public:

    DxInit();
    ~DxInit();

    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void ResetDevice();
    int Run();

    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


private:

    HWND hwnd;
    WNDCLASSEX wc;

};