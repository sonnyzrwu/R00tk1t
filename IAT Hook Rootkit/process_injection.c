#include "process_injection.h"

HANDLE ProcessInjection( DWORD PID, SIZE_T SIZE, LPCSTR pDllPath, LPCSTR moduleName, LPCSTR processName) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (hProcess == NULL) {
        printf("Failed to open process with PID %d. Error: %d\n", PID, GetLastError());
        return NULL;
    }

    LPVOID pBaseAddress = VirtualAllocEx(hProcess, NULL, SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (pBaseAddress == NULL) {
        printf("Failed to allocate memory in the target process. Error: %d\n", GetLastError());
        CloseHandle(hProcess);
        return NULL;
    }
    // Write the DLL path to the allocated memory in the target process
    BOOL isValid = WriteProcessMemory(hProcess, pBaseAddress, pDllPath, strlen(pDllPath) + 1, NULL); 

    if (!isValid) {
        printf("Failed to write to process memory. Error: %d\n", GetLastError());
        return NULL;
    }

    //HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    //FARPROC pLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryA");
    HMODULE hKernel32 = GetModuleHandleA(moduleName);
    if (hKernel32 == NULL){
        printf("Failed to GetModuleHandleA. Error: %d\n", GetLastError());
        return NULL;
    }
    FARPROC pLoadLibrary = GetProcAddress(hKernel32, processName);

    if (pLoadLibrary == NULL) {
        printf("Failed to get address of LoadLibraryA. Error: %d\n", GetLastError());
        return NULL;
    }

    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) pLoadLibrary, pBaseAddress, 0 , NULL );
    return hRemoteThread;
}

