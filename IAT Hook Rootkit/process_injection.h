#include <iostream>
#include <windows.h>
#include <TlHelp32.h>


HANDLE ProcessInjection( DWORD PID, SIZE_T SIZE, LPCSTR pDllPath, LPCSTR moduleName, LPCSTR processName);